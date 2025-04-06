#include "Renderer.h"
#include "Engine/Window.h"

namespace Graphics {
	Renderer::Renderer(Engine::Window* win) : win(win), device(nullptr), context(nullptr), swapChain(nullptr),
		renderTargetView(nullptr), depthStencilBuffer(nullptr), depthStencilView(nullptr), viewport({}) {
	}
	Renderer::~Renderer() {
		Shutdown();
	}
	bool Renderer::Initialize() {
		HRESULT hr;

		RECT rect = win->GetWindowRect();

		// Create device and swap chain
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = static_cast<UINT>(rect.right - rect.left);
		swapChainDesc.BufferDesc.Height = static_cast<UINT>(rect.bottom - rect.top);
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = win->GetHandle();
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = TRUE;
		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&swapChain,
			&device,
			nullptr,
			&context
		);

		if (FAILED(hr)) {
			MessageBoxA(nullptr, "Failed to create device and swap chain", "Error", MB_ICONERROR);
			return false;
		}

		// Create viewport
		BuildViewport();
		// Create render target view
		BuildRenderTarget();
		// Create depth stencil view
		BuildDepthStencil();
		// Create rasterizer state
		BuildRasterizer();
		// Create blend state
		BuildBlendState();


		// Redraw the window when sizing
		win->AddMessageHook(WM_SIZE, [this](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			Resize();
			return 0;
			});
	}

	void Renderer::BeginRender() {
		ClearTargetAndStencil();
	}

	void Renderer::EndRender() {
		Present();
	}

	void Renderer::Resize() {
		ResizeSwapChain();
		BeginRender();
		EndRender();
	}

	void Renderer::Shutdown() {
		Cleanup();
	}

	void Renderer::BuildViewport() {
		RECT rect = win->GetWindowRect();
		viewport.Width = static_cast<float>(rect.right - rect.left);
		viewport.Height = static_cast<float>(rect.bottom - rect.top);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		context->RSSetViewports(1, &viewport);
	}

	void Renderer::BuildRenderTarget() {
		renderTargetView.Reset();

		ComPtr<ID3D11Texture2D> backBuffer;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
		context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);
	}

	void Renderer::BuildDepthStencil() {
		depthStencilBuffer.Reset();
		depthStencilView.Reset();

		RECT rect = win->GetWindowRect();

		D3D11_TEXTURE2D_DESC depthStencilDesc = {};
		depthStencilDesc.Width = static_cast<UINT>(rect.right - rect.left);
		depthStencilDesc.Height = static_cast<UINT>(rect.bottom - rect.top);
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, &depthStencilView);
		context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	}

	void Renderer::BuildRasterizer() {
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.DepthClipEnable = TRUE;
		ID3D11RasterizerState* rasterizerState;
		device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
		context->RSSetState(rasterizerState);
		rasterizerState->Release();
	}

	void Renderer::BuildBlendState() {
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[0];
		rtBlendDesc.BlendEnable = TRUE;
		rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		ID3D11BlendState* blendState;
		device->CreateBlendState(&blendDesc, &blendState);
		context->OMSetBlendState(blendState, nullptr, 0xffffffff);
		blendState->Release();
	}

	void Renderer::ClearTargetAndStencil() {
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		context->ClearRenderTargetView(renderTargetView.Get(), clearColor);
		context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void Renderer::Present() {
		swapChain->Present(1, 0);
	}

	void Renderer::ResizeSwapChain() {
		if (context) {
			context->Flush();
			context->ClearState();
		}

		if (renderTargetView) {
			renderTargetView.Reset();
		}

		if (depthStencilView) {
			depthStencilView.Reset();
		}

		if (depthStencilBuffer) {
			depthStencilBuffer.Reset();
		}

		if (swapChain) {
			RECT rect = win->GetWindowRect();
			UINT width = static_cast<UINT>(rect.right - rect.left);
			UINT height = static_cast<UINT>(rect.bottom - rect.top);
			swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
			BuildRenderTarget();
			BuildDepthStencil();
			BuildViewport();
		}
	}

	void Renderer::Cleanup() {
		renderTargetView.Reset();
		depthStencilView.Reset();
		depthStencilBuffer.Reset();
		swapChain.Reset();
		context.Reset();
		device.Reset();
	}
}