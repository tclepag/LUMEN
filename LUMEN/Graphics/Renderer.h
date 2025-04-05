#pragma once

#include <d3d11.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace Graphics {
	class Renderer {
	public:
		Renderer(HWND hWnd);
		~Renderer();
		bool Initialize();
		void Render();
		void Shutdown();
	private:
		HWND hWnd;
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<IDXGISwapChain> swapChain;
		ComPtr<ID3D11RenderTargetView> renderTargetView;
		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		ComPtr<ID3D11DepthStencilView> depthStencilView;
	};
}