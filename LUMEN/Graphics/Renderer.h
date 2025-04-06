#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace Engine {
	class Window;
}

namespace Graphics {
	class Renderer {
	public:
		Renderer(Engine::Window* win);
		~Renderer();
		bool Initialize();
		void BeginRender();
		void EndRender();
		void Resize();
		void Shutdown();
	private:
		// Private members
		Engine::Window* win;
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<IDXGISwapChain> swapChain;
		ComPtr<ID3D11RenderTargetView> renderTargetView;
		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		ComPtr<ID3D11DepthStencilView> depthStencilView;
		D3D11_VIEWPORT viewport;

		int resizeDrawPerSecond = 150;

		// Private methods
		void BuildViewport();
		void BuildRenderTarget();
		void BuildDepthStencil();
		void BuildRasterizer();
		void BuildBlendState();
		void ClearTargetAndStencil();
		void Present();
		void ResizeSwapChain();
		void Cleanup();
	};
}