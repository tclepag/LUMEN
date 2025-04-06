#pragma once
#include <Windows.h>
#include <string>

#include <ThirdParty/Net/include/coreclr_delegates.h>
#include <ThirdParty/Net/include/nethost.h>
#include <ThirdParty/Net/include/hostfxr.h>

namespace Engine {
    class NetRuntime {
    public:
        NetRuntime();
        ~NetRuntime();

        // Initialize the .NET runtime with paths to required files
        bool Initialize(const std::wstring& runtimeConfigPath, const std::wstring& assemblyPath);

        // Load a method from the assembly
        template<typename T>
        bool LoadMethod(const std::wstring& typeName, const std::wstring& methodName,
            const std::wstring& delegateTypeName, T* methodDelegate);

        // Shutdown the runtime
        void Shutdown();

        // Check if runtime is initialized
        bool IsInitialized() const { return m_Initialized; }

    private:
        // Runtime state
        bool m_Initialized;
        hostfxr_handle m_HostContext;
        load_assembly_and_get_function_pointer_fn m_LoadAssembly;
        std::wstring m_AssemblyPath;

        // Helper methods for initialization
        bool GetHostFxrPath(std::wstring& hostFxrPath);
        bool LoadHostFxr(const std::wstring& hostFxrPath);
    };

    // Template implementation
    template<typename T>
    bool NetRuntime::LoadMethod(const std::wstring& typeName, const std::wstring& methodName,
        const std::wstring& delegateTypeName, T* methodDelegate) {
        if (!m_Initialized || m_LoadAssembly == nullptr) {
            return false;
        }

        int rc = m_LoadAssembly(
            m_AssemblyPath.c_str(),
            typeName.c_str(),
            methodName.c_str(),
            delegateTypeName.c_str(),
            nullptr,
            (void**)methodDelegate);

        return (rc == 0 && *methodDelegate != nullptr);
    }
}