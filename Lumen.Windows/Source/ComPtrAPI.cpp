#include "pch.h"
#include "Include/ComPtrAPI.h"

using Microsoft::WRL::ComPtr;

extern "C" {
    __declspec(dllexport) void* __cdecl Native_CreateCom() {
        ComPtr<IUnknown> unknown;
        return nullptr;
    }

    __declspec(dllexport) ULONG __cdecl Native_AddRef(void* com)
    {
        ComPtr<IUnknown> unknown = static_cast<ComPtr<IUnknown>>(com);
        if (unknown) {
            return unknown->AddRef();
        }
    }

    __declspec(dllexport) void __cdecl Native_ReleaseCom(void* com) {
        ComPtr<IUnknown> unknown = static_cast<ComPtr<IUnknown>>(com);
        if (unknown) {
            unknown->Release();
            unknown = nullptr;
        }
    }

    __declspec(dllexport) void __cdecl Native_ResetCom(void* com) {
        ComPtr<IUnknown> unknown = static_cast<ComPtr<IUnknown>>(com);
        if (unknown) {
            unknown->Release();
        }
    }
}
