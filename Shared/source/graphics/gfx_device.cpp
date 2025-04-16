#include "graphics/gfx_device.h"

namespace gfx {
	bool GFXDevice::Init(HWND hWnd) {
		// Enable debug layer, only if we are debugging in MSVC
#if defined(_DEBUG)
		ComPtr<ID3D12Debug> debug;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
			debug->EnableDebugLayer();
		}
#endif

		// Create DXGI factory
		ComPtr<IDXGIFactory6> factory;
		if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)))) {
			return false;
		}

		// Create device
		if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device)))) {
			return false;
		}

		// Create command queue
		D3D12_COMMAND_QUEUE_DESC qDesc{};
		qDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		qDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		if (FAILED(_device->CreateCommandQueue(&qDesc, IID_PPV_ARGS(&_cmdQ)))) {
			return false;
		}

		// Create swapChain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Width = 800;
		swapChainDesc.Height = 600;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		if (FAILED(factory->CreateSwapChainForHwnd(
			_cmdQ.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &_swapChain
		))) {
			return false;
		}

		return true;
	}
}