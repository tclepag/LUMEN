#pragma once

#include <d3d11.h>
#include <dxgi1_6.h>
#include "Include/ComPtrAPI.h"

extern "C" {
	__declspec(dllexport) HRESULT __cdecl Native_InitializeD3D11(
		HWND hWnd,
		void** device,
		void** context,
		void** swapChain
	);
}