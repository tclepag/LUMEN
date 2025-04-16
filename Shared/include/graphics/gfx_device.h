#pragma once

// D3D12 Libraries
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

// D3D12 Headers
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace gfx {
	class GFXDevice {
	public:
		bool Init(HWND hWnd);
		void StartRender();
		void EndRender();
		void Destroy();
	private:
		static const UINT BACKBUFFER_COUNT = 2;

		// Pipeline objects
		D3D12_VIEWPORT _viewport;
		D3D12_RECT _rect;
		ComPtr<ID3D12Device> _device;
		ComPtr<IDXGISwapChain1> _swapChain;
		ComPtr<ID3D12Resource> _renderTargets[BACKBUFFER_COUNT];
		ComPtr<ID3D12CommandAllocator> _cmdAllocator;
		ComPtr<ID3D12CommandQueue> _cmdQ;
		ComPtr<ID3D12RootSignature> _rootSignature;
		ComPtr<ID3D12DescriptorHeap> _rtvHeap;
		ComPtr<ID3D12PipelineState> _pipelineState;
		ComPtr<ID3D12GraphicsCommandList> _commandList;
		UINT _rtvDescriptorSize;

		// Synchronization objects
		UINT _frame;
		HANDLE _fenceEvent;
		ComPtr<ID3D12Fence> _fence;
		UINT64 _fenceVal;

		void CreateDevice();
		void LoadPipeline();
		void LoadContent();
		void GenCmdList();
		void WaitForPrevFrame();
	};
}