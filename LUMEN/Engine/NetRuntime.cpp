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

        // Verify file existence
        if (!std::filesystem::exists(assemblyPath)) {
            LogError(L"Assembly file not found: " + assemblyPath);
            return false;
        }

        if (!std::filesystem::exists(runtimeConfigPath)) {
            LogError(L"Runtime config file not found: " + runtimeConfigPath);
            return false;
        }

        // Get hostfxr path
        std::wstring hostFxrPath;
        if (!GetHostFxrPath(hostFxrPath)) {
            LogError(L"Failed to get hostfxr path");
            return false;
        }

        OutputDebugStringW((L"Using hostfxr path: " + hostFxrPath).c_str());

        // Load hostfxr library
        if (!LoadHostFxr(hostFxrPath)) {
            LogError(L"Failed to load hostfxr library");
            return false;
        }

        // Get function pointers
        HMODULE hostfxrLib = GetModuleHandleW(L"hostfxr.dll");
        if (hostfxrLib == NULL) {
            LogError(L"Failed to get module handle for hostfxr.dll");
            return false;
        }

        auto init_fptr = (hostfxr_initialize_for_runtime_config_fn)GetProcAddress(hostfxrLib, "hostfxr_initialize_for_runtime_config");
        auto get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)GetProcAddress(hostfxrLib, "hostfxr_get_runtime_delegate");
        auto close_fptr = (hostfxr_close_fn)GetProcAddress(hostfxrLib, "hostfxr_close");

        if (!init_fptr || !get_delegate_fptr || !close_fptr) {
            LogError(L"Failed to get function pointers from hostfxr.dll");
            return false;
        }

        // Initialize runtime using config
        OutputDebugStringW((L"Initializing runtime with config: " + runtimeConfigPath).c_str());
        int rc = init_fptr(runtimeConfigPath.c_str(), nullptr, &m_HostContext);
        if (rc != 0 || m_HostContext == nullptr) {
            LogError(L"Failed to initialize runtime. Error code: " + std::to_wstring(rc));
            return false;
        }

        // Get function pointer loader
        rc = get_delegate_fptr(
            m_HostContext,
            hdt_load_assembly_and_get_function_pointer,
            (void**)&m_LoadAssembly);

        if (rc != 0 || m_LoadAssembly == nullptr) {
            LogError(L"Failed to get load_assembly_and_get_function_pointer delegate. Error code: " + std::to_wstring(rc));
            close_fptr(m_HostContext);
            m_HostContext = nullptr;
            return false;
        }

        m_Initialized = true;
        OutputDebugStringA("Runtime initialized successfully\n");
        return true;
    }

    template<typename TDelegate>
    bool NetRuntime::LoadMethod(const std::wstring& typeName, const std::wstring& methodName,
        TDelegate* methodDelegate) {
        if (!m_Initialized || m_LoadAssembly == nullptr) {
            OutputDebugStringA("Runtime not initialized or LoadAssembly function pointer is null\n");
            return false;
        }

        // Get appropriate delegate type name
        std::wstring delegateTypeName = GetDelegateTypeName<TDelegate>();

        // Debug output
        std::wstring debugMsg = L"Loading method: " + methodName +
            L" from type: " + typeName +
            L" with delegate: " + delegateTypeName + L"\n";
        OutputDebugStringW(debugMsg.c_str());

        int rc = m_LoadAssembly(
            m_AssemblyPath.c_str(),
            typeName.c_str(),
            methodName.c_str(),
            delegateTypeName.c_str(),
            nullptr,
            (void**)methodDelegate);

        if (rc != 0 || *methodDelegate == nullptr) {
            std::wstring errorMsg = L"Failed to load method: " + methodName +
                L" of type: " + typeName +
                L" with HRESULT: 0x" + std::to_wstring(rc) + L"\n";

            // Interpret common error codes
            switch (rc) {
            case -2146233052: // 0x80131534 - COR_E_MISSINGMETHOD
                errorMsg += L"Method not found in the specified type\n";
                break;
            case -2146233054: // 0x80131522 - COR_E_TYPELOAD
                errorMsg += L"Type not found or could not be loaded\n";
                break;
            case -2146233048: // 0x80131538 - COR_E_TARGETINVOCATION
                errorMsg += L"Exception occurred during method invocation\n";
                break;
            }

            OutputDebugStringW(errorMsg.c_str());
            return false;
        }

        return true;
    }

    template<typename TReturn, typename... TArgs>
    bool NetRuntime::CallMethod(void* instance, const std::wstring& typeName,
        const std::wstring& methodName, TReturn* result, TArgs... args) {
        if (!m_Initialized) {
            OutputDebugStringA("Runtime not initialized\n");
            return false;
        }

        // Define the delegate type based on return type and parameters
        using MethodDelegate = TReturn(*)(void*, TArgs...);
        MethodDelegate methodDelegate = nullptr;

        // Load the method with appropriate delegate type
        if (!LoadMethod(typeName, methodName, &methodDelegate)) {
            return false;
        }

        OutputDebugStringA(("Calling method: " + std::string(methodName.begin(), methodName.end()) + "\n").c_str());

        try {
            // Handle void return type differently from other return types
            if constexpr (std::is_void_v<TReturn>) {
                // For void return type, just call the method
                methodDelegate(instance, args...);
            }
            else {
                // For non-void return types, store the result
                if (result) {
                    *result = methodDelegate(instance, args...);
                    OutputDebugStringA("Method returned successfully\n");
                }
            }
        }
        catch (...) {
            OutputDebugStringA("Exception occurred when calling method\n");
            return false;
        }

        return true;
    }

    template<typename T>
    bool NetRuntime::CreateInstance(const std::wstring& typeName, T** instance) {
        if (!m_Initialized) {
            OutputDebugStringA("Runtime not initialized\n");
            return false;
        }

        // Load factory method
        using FactoryDelegate = T * (*)();
        FactoryDelegate factory = nullptr;

        OutputDebugStringW((L"Creating instance of type: " + typeName).c_str());

        if (!LoadMethod(typeName, L"CreateInstance", &factory)) {
            OutputDebugStringA("Failed to load factory method\n");
            return false;
        }

        try {
            *instance = factory();

            if (*instance == nullptr) {
                OutputDebugStringA("Factory method returned null\n");
                return false;
            }

            OutputDebugStringA("Instance created successfully\n");
            return true;
        }
        catch (...) {
            OutputDebugStringA("Exception occurred when creating instance\n");
            return false;
        }
    }

    bool NetRuntime::DumpAssemblyInfo() {
        if (!m_Initialized) {
            OutputDebugStringA("Runtime not initialized\n");
            return false;
        }

        using StringDelegate = const char* (*)();
        StringDelegate getInfoDelegate = nullptr;

        if (!LoadMethod(L"LSHARP.Diagnostics", L"GetAssemblyInfo", &getInfoDelegate)) {
            OutputDebugStringA("Failed to load GetAssemblyInfo method. Ensure LSHARP.Diagnostics class exists.\n");

            // Try to find the class in the Game type
            if (!LoadMethod(L"LSHARP.Game", L"GetTypeInfo", &getInfoDelegate)) {
                OutputDebugStringA("Failed to load GetTypeInfo method from Game class\n");
                return false;
            }
        }

        try {
            const char* info = getInfoDelegate();
            if (info == nullptr) {
                OutputDebugStringA("GetAssemblyInfo returned null\n");
                return false;
            }

            OutputDebugStringA("Assembly Info:\n");
            OutputDebugStringA(info);
            return true;
        }
        catch (const std::exception& ex) {
            OutputDebugStringA(("Exception occurred when getting assembly info: " + std::string(ex.what()) + "\n").c_str());
            return false;
        }
        catch (...) {
            OutputDebugStringA("Unknown exception occurred when getting assembly info\n");
            return false;
        }
    }


    void NetRuntime::Shutdown() {
        if (m_HostContext != nullptr) {
            HMODULE hostfxrLib = GetModuleHandleW(L"hostfxr.dll");
            if (hostfxrLib != NULL) {
                hostfxr_close_fn close_fptr = (hostfxr_close_fn)GetProcAddress(hostfxrLib, "hostfxr_close");
                if (close_fptr != nullptr) {
                    close_fptr(m_HostContext);
                }
            }
            m_HostContext = nullptr;
        }

        m_LoadAssembly = nullptr;
        m_Initialized = false;
        OutputDebugStringA("Runtime shut down\n");
    }

    bool NetRuntime::GetHostFxrPath(std::wstring& hostFxrPath) {
        // Buffer for hostfxr path
        char_t buffer[MAX_PATH];
        size_t buffer_size = sizeof(buffer) / sizeof(char_t);

        // Get hostfxr path, this looks for dotnet installation on the system
        int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);
        if (rc != 0) {
            LogError(L"get_hostfxr_path failed with code: " + std::to_wstring(rc));
            return false;
        }

        hostFxrPath = buffer;
        return true;
    }

    bool NetRuntime::LoadHostFxr(const std::wstring& hostFxrPath) {
        // Load hostfxr library
        HMODULE lib = LoadLibraryW(hostFxrPath.c_str());
        if (lib == nullptr) {
            DWORD error = GetLastError();
            LogError(L"LoadLibraryW failed with error code: " + std::to_wstring(error));
            return false;
        }
        return true;
    }

    void NetRuntime::LogError(const std::wstring& message) {
        DWORD errorCode = GetLastError();
        LPVOID errorMsg = nullptr;

        if (errorCode != 0) {
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&errorMsg,
                0,
                NULL
            );
        }

        std::wstring fullMessage = message;
        if (errorCode != 0 && errorMsg != nullptr) {
            fullMessage += L" - Error code: " + std::to_wstring(errorCode) + L" - " + (LPWSTR)errorMsg;
            LocalFree(errorMsg);
        }

        OutputDebugStringW(fullMessage.c_str());
    }

    // Template specializations for delegate type names
    template<>
    std::wstring NetRuntime::GetDelegateTypeName<void(*)()>() {
        return L"System.Action";
    }

    template<>
    std::wstring NetRuntime::GetDelegateTypeName<int(*)()>() {
        return L"System.Func`1[System.Int32]";
    }

    template<>
    std::wstring NetRuntime::GetDelegateTypeName<void(*)(void*)>() {
        return L"System.Action`1[System.Object]";
    }

    template<>
    std::wstring NetRuntime::GetDelegateTypeName<int(*)(void*)>() {
        return L"System.Func`2[System.Object,System.Int32]";
    }

    template<>
    std::wstring NetRuntime::GetDelegateTypeName<void* (*)()>() {
        return L"System.Func`1[System.Object]";
    }

    template<>
    std::wstring NetRuntime::GetDelegateTypeName<const char* (*)()>() {
        return L"System.Func`1[System.String]";
    }

    // Template instantiations
    template bool NetRuntime::LoadMethod<int(*)()>(const std::wstring&, const std::wstring&, int(**)());
    template bool NetRuntime::LoadMethod<void(*)(void*)>(const std::wstring&, const std::wstring&, void(**)(void*));
    template bool NetRuntime::LoadMethod<int(*)(void*)>(const std::wstring&, const std::wstring&, int(**)(void*));
    template bool NetRuntime::LoadMethod<void* (*)()>(const std::wstring&, const std::wstring&, void* (**)());
    template bool NetRuntime::LoadMethod<const char* (*)()>(const std::wstring&, const std::wstring&, const char* (**)());

    template bool NetRuntime::CallMethod<int>(void*, const std::wstring&, const std::wstring&, int*);
    template bool NetRuntime::CallMethod<void>(void*, const std::wstring&, const std::wstring&, void*);

    template bool NetRuntime::CreateInstance<void>(const std::wstring&, void**);
}
