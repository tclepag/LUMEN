#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <Unknwn.h>

extern "C" {
	__declspec(dllexport) void* __cdecl Native_CreateCom();
	__declspec(dllexport) ULONG __cdecl Native_AddRef(void* com);
	__declspec(dllexport) void __cdecl Native_ReleaseCom(void* com);
	__declspec(dllexport) void __cdecl Native_ResetCom(void* com);
}