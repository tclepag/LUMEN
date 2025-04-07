#include "Project.h"
#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <sstream>

namespace Engine {
    Project::Project() : netRuntime(std::make_unique<NetRuntime>()) {
    }

    bool Project::Init() {
        // Find assembly paths
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
        std::filesystem::path assemblyPath = exeDir / L"LSHARP.dll";
        std::filesystem::path configPath = exeDir / L"LSHARP.runtimeconfig.json";

        // Output paths for debugging
        OutputDebugStringW((L"Assembly path: " + assemblyPath.wstring() + L"\n").c_str());
        OutputDebugStringW((L"Config path: " + configPath.wstring() + L"\n").c_str());

        // Check file existence
        if (!std::filesystem::exists(assemblyPath)) {
            OutputDebugStringW((L"Assembly file not found: " + assemblyPath.wstring() + L"\n").c_str());
            return false;
        }

        if (!std::filesystem::exists(configPath)) {
            OutputDebugStringW((L"Runtime config file not found: " + configPath.wstring() + L"\n").c_str());
            return false;
        }

        // Load assembly
        if (!LoadAssembly(assemblyPath.wstring(), configPath.wstring())) {
            return false;
        }

        // Dump assembly info for diagnostics
        DumpAssemblyInfo();

        // Test if we can access game objects with a static method
        int result;
        if (!netRuntime->CallMethod(nullptr, L"LSHARP.Game", L"Hello", &result)) {
            OutputDebugStringA("Failed to call Hello method\n");
            return false;
        }

        OutputDebugStringA(("Hello method returned: " + std::to_string(result) + "\n").c_str());

        // Create game instance
        if (!CreateGameInstance()) {
            return false;
        }

        // Call Initialize method on game instance
        if (!CallGameMethod(L"Initialize", &result)) {
            return false;
        }

        OutputDebugStringA("Game initialized successfully\n");
        return true;
    }

    bool Project::Run() {
        int result;
        return CallGameMethod(L"Start", &result);
    }

    bool Project::Update() {
        int result;
        return CallGameMethod(L"Update", &result);
    }

    bool Project::Render() {
        int result;
        return CallGameMethod(L"Render", &result);
    }

    bool Project::Shutdown() {
        // Call Shutdown method on game instance
        int result;
        if (gameInstance) {
            CallGameMethod(L"Shutdown", &result);
        }

        // Clean up resources
        gameInstance = nullptr;

        // Shutdown the runtime
        if (netRuntime && netRuntime->IsInitialized()) {
            netRuntime->Shutdown();
        }

        return true;
    }

    bool Project::DumpAssemblyInfo() {
        if (!netRuntime || !netRuntime->IsInitialized()) {
            OutputDebugStringA("Runtime not initialized for assembly info dump\n");
            return false;
        }

        return netRuntime->DumpAssemblyInfo();
    }

    // Helper methods

    bool Project::LoadAssembly(const std::wstring& assemblyPath, const std::wstring& configPath) {
        // Initialize the .NET runtime
        OutputDebugStringA("Initializing .NET runtime...\n");
        if (!netRuntime->Initialize(configPath, assemblyPath)) {
            OutputDebugStringA("Failed to initialize .NET runtime\n");
            return false;
        }

        OutputDebugStringA("Initialized .NET runtime successfully\n");
        return true;
    }

    bool Project::CreateGameInstance() {
        // Create an instance of the game class
        OutputDebugStringA("Creating game instance...\n");
        if (!netRuntime->CreateInstance(L"LSHARP.Game", &gameInstance)) {
            OutputDebugStringA("Failed to create game instance\n");
            return false;
        }

        OutputDebugStringA("Created game instance successfully\n");
        return true;
    }

    bool Project::CallGameMethod(const std::wstring& methodName, int* result) {
        // Call a method on the game instance
        if (!gameInstance) {
            OutputDebugStringA("Cannot call method on null game instance\n");
            return false;
        }

        // Use the improved CallMethod template from NetRuntime
        OutputDebugStringA(("Calling game method: " + std::string(methodName.begin(), methodName.end()) + "\n").c_str());

        if (!netRuntime->CallMethod(gameInstance, L"LSHARP.Game", methodName, result)) {
            std::string errorMsg = "Failed to call " + std::string(methodName.begin(), methodName.end()) + " method\n";
            OutputDebugStringA(errorMsg.c_str());
            return false;
        }

        return true;
    }
}
