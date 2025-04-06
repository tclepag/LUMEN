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
        std::wstring hostFxrPath;
        if (!GetHostFxrPath(hostFxrPath)) {
            std::cerr << "Failed to get hostfxr path" << std::endl;
            return false;
        }

        // Load hostfxr library
        if (!LoadHostFxr(hostFxrPath)) {
            std::cerr << "Failed to load hostfxr" << std::endl;
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
            std::cerr << "Failed to get hostfxr function pointers" << std::endl;
            return false;
        }

        // Initialize runtime using config
        int rc = init_fptr(runtimeConfigPath.c_str(), nullptr, &m_HostContext);
        if (rc != 0 || m_HostContext == nullptr) {
            std::cerr << "Failed to initialize hostfxr: " << rc << std::endl;
            return false;
        }

        // Get function pointer loader
        rc = get_delegate_fptr(
            m_HostContext,
            hdt_load_assembly_and_get_function_pointer,
            (void**)&m_LoadAssembly);

        if (rc != 0 || m_LoadAssembly == nullptr) {
            std::cerr << "Failed to get function pointer loader: " << rc << std::endl;
            close_fptr(m_HostContext);
            m_HostContext = nullptr;
            return false;
        }

        m_Initialized = true;
        return true;
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
}