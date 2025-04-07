#include "NetRuntime.h"
#include <iostream>

namespace Engine {
    NetRuntime::NetRuntime() :
        m_Initialized(false),
        m_HostContext(nullptr),
        m_LoadAssembly(nullptr) {
    }

    NetRuntime::~NetRuntime() {
        Shutdown();
    }

    bool NetRuntime::Initialize(const std::wstring& runtimeConfigPath, const std::wstring& assemblyPath) {
        // Store assembly path for later use
        m_AssemblyPath = assemblyPath;

        // Get hostfxr path
        // Get hostfxr path
        std::wstring hostFxrPath;
        if (!GetHostFxrPath(hostFxrPath)) {
			DWORD errorCode = GetLastError();
			LPVOID errorMsg;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errorCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPWSTR)&errorMsg,
				0,
				NULL
			);
			std::wstring errorMessage = L"Failed to get hostfxr path: " + std::to_wstring(errorCode) + L" - " + (LPWSTR)errorMsg;
			OutputDebugStringW(errorMessage.c_str());
			LocalFree(errorMsg);
            return false;
        }

        // Load hostfxr library
        if (!LoadHostFxr(hostFxrPath)) {
            DWORD errorCode = GetLastError();
            LPVOID errorMsg;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&errorMsg,
                0,
                NULL
            );
			std::wstring errorMessage = L"Failed to load hostfxr: " + std::to_wstring(errorCode) + L" - " + (LPWSTR)errorMsg;
			OutputDebugStringW(errorMessage.c_str());
            LocalFree(errorMsg);
            return false;
        }


        // Get function pointers
        auto init_fptr = (hostfxr_initialize_for_runtime_config_fn)GetProcAddress(
            GetModuleHandleW(L"hostfxr.dll"),
            "hostfxr_initialize_for_runtime_config");

        auto get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)GetProcAddress(
            GetModuleHandleW(L"hostfxr.dll"),
            "hostfxr_get_runtime_delegate");

        auto close_fptr = (hostfxr_close_fn)GetProcAddress(
            GetModuleHandleW(L"hostfxr.dll"),
            "hostfxr_close");

        if (!init_fptr || !get_delegate_fptr || !close_fptr) {
			DWORD errorCode = GetLastError();
			LPVOID errorMsg;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errorCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPWSTR)&errorMsg,
				0,
				NULL
			);
			std::wstring errorMessage = L"Failed to get function pointers: " + std::to_wstring(errorCode) + L" - " + (LPWSTR)errorMsg;
			OutputDebugStringW(errorMessage.c_str());
			LocalFree(errorMsg);
            return false;
        }

        // Initialize runtime using config
        int rc = init_fptr(runtimeConfigPath.c_str(), nullptr, &m_HostContext);
        if (rc != 0 || m_HostContext == nullptr) {
            DWORD errorCode = GetLastError();
            LPVOID errorMsg;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&errorMsg,
                0,
                NULL
            );
            std::wstring errorMessage = L"Failed to get function pointers: " + std::to_wstring(errorCode) + L" - " + (LPWSTR)errorMsg;
            OutputDebugStringW(errorMessage.c_str());
            LocalFree(errorMsg);
            return false;
        }

        // Get function pointer loader
        rc = get_delegate_fptr(
            m_HostContext,
            hdt_load_assembly_and_get_function_pointer,
            (void**)&m_LoadAssembly);

        if (rc != 0 || m_LoadAssembly == nullptr) {
            DWORD errorCode = GetLastError();
            LPVOID errorMsg;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&errorMsg,
                0,
                NULL
            );
            std::wstring errorMessage = L"Failed to get function pointers: " + std::to_wstring(errorCode) + L" - " + (LPWSTR)errorMsg;
            OutputDebugStringW(errorMessage.c_str());
            LocalFree(errorMsg);
            close_fptr(m_HostContext);
            m_HostContext = nullptr;
            return false;
        }

        m_Initialized = true;
        return true;
    }

    template<typename T>
    bool NetRuntime::LoadMethod(const std::wstring& typeName, const std::wstring& methodName,
        const std::wstring& delegateTypeName, T* methodDelegate) {
        if (!m_Initialized || m_LoadAssembly == nullptr) {
            OutputDebugStringA("Runtime not initialized or LoadAssembly function pointer is null\n");
            return false;
        }

        int rc = m_LoadAssembly(
            m_AssemblyPath.c_str(),
            typeName.c_str(),
            methodName.c_str(),
            delegateTypeName.c_str(),
            nullptr,
            (void**)methodDelegate);

        if (rc != 0 || *methodDelegate == nullptr) {
            std::string errorMsg = "Failed to load method: " + std::string(methodName.begin(), methodName.end()) +
                                   " of type: " + std::string(typeName.begin(), typeName.end()) +
                                   " with HRESULT: " + std::to_string(rc) + "\n";
            OutputDebugStringA(errorMsg.c_str());
            return false;
        }

        return true;
    }

	template<typename RETURN, typename... ARGS>
	bool NetRuntime::LoadAndCallMethod(void* instance, const std::wstring& typeName, const std::wstring& methodName, RETURN* result, ARGS... args) {
		if (!m_Initialized || m_LoadAssembly == nullptr) {
			return false;
		}
		// Load the method
		using MethodDelegate = RETURN(*)(void*, ARGS...);
		MethodDelegate methodDelegate = nullptr;
		if (!LoadMethod(typeName, methodName, L"System.Func`1[System.Int32]", &methodDelegate)) {
			return false;
		}
		// Call the method
		*result = methodDelegate(instance, args...);
		return true;
	}

    template <typename T>
    bool NetRuntime::CreateInstance(const std::wstring& typeName, T** instance) {
        // Load factory method
        using FactoryDelegate = void* (*)();
        FactoryDelegate factory = nullptr;

        if (!LoadMethod(typeName, L"CreateInstance", L"System.Func`1[LSHARP.Game]", &factory)) {
            OutputDebugStringA("Failed to load factory method\n");
            return false;
        }

        *instance = static_cast<T*>(factory());
        return *instance != nullptr;
    }


    void NetRuntime::Shutdown() {
        if (m_HostContext != nullptr) {
            hostfxr_close_fn close_fptr = (hostfxr_close_fn)GetProcAddress(
                GetModuleHandleW(L"hostfxr.dll"),
                "hostfxr_close");

            if (close_fptr != nullptr) {
                close_fptr(m_HostContext);
            }
            m_HostContext = nullptr;
        }

        m_LoadAssembly = nullptr;
        m_Initialized = false;
    }

    bool NetRuntime::GetHostFxrPath(std::wstring& hostFxrPath) {
        // Buffer for hostfxr path
        char_t buffer[MAX_PATH];
        size_t buffer_size = sizeof(buffer) / sizeof(char_t);

        // Get hostfxr path, this looks for dotnet installation on the system
        int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);
        if (rc != 0) {
            return false;
        }

        hostFxrPath = buffer;
        return true;
    }

    bool NetRuntime::LoadHostFxr(const std::wstring& hostFxrPath) {
        // Load hostfxr library
        HMODULE lib = LoadLibraryW(hostFxrPath.c_str());
        return (lib != nullptr);
    }

    template bool NetRuntime::LoadAndCallMethod<int>(void* instance, const std::wstring& typeName, const std::wstring& methodName, int* result);
    template bool NetRuntime::CreateInstance<void>(const std::wstring& typeName, void** instance);
}