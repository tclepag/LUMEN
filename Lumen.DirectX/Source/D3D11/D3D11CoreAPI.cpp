#include "pch.h"
#include "Include/D3D11/D3D11CoreAPI.h"

extern "C" {
    HRESULT __cdecl Native_InitializeD3D11(HWND hWnd, void** device, void** context, void** swapChain)
    {
        ID3D11Device* d3dDevice = nullptr;
        ID3D11DeviceContext* d3dContext = nullptr;
        IDXGISwapChain* d3dSwapChain = nullptr;

        HRESULT hr;

        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &d3dDevice,
            nullptr,
            &d3dContext
        );

        if (FAILED(hr))
            return hr;

        DXGI_SWAP_CHAIN_DESC desc{};


        IDXGIDevice* dxgiDevice = nullptr;
        hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
        if (FAILED(hr)) {
            d3dDevice->Release();
            d3dContext->Release();
            return hr;
        }

        IDXGIAdapter* adapter = nullptr;
        hr = dxgiDevice->GetAdapter(&adapter);
        dxgiDevice->Release();
        if (FAILED(hr)) {
            d3dDevice->Release();
            d3dContext->Release();
            return hr;
        }

        IDXGIFactory* factory = nullptr;
        hr = adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);
        adapter->Release();
        if (FAILED(hr)) {
            d3dDevice->Release();
            d3dContext->Release();
            return hr;
        }

        hr = factory->CreateSwapChain(d3dDevice, &desc, &d3dSwapChain);
        factory->Release();
        if (FAILED(hr)) {
            d3dDevice->Release();
            d3dContext->Release();
            return hr;
        }

        *device = d3dDevice;
        *context = d3dContext;
        *swapChain = d3dSwapChain;
    }

}