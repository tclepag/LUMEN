#include "Project.h"
#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <sstream>

namespace Engine {
    Project::Project() : netRuntime(std::make_unique<NetRuntime>()) {
    }

    void Project::Init() {
        // Initialize the .NET runtime
        std::wstring assemblyPath = L"LSHARP.dll";
        std::wstring runtimeConfigPath = L"LSHARP.runtimeconfig.json";

        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
        std::filesystem::path runtimeConfigFullPath = exeDir / runtimeConfigPath;
		std::filesystem::path assemblyFullPath = exeDir / assemblyPath;

		// Check if the assembly file exists
		if (!std::filesystem::exists(assemblyFullPath)) {
			std::wstringstream ss;
			ss << L"Assembly file not found: " << assemblyFullPath << std::endl;
			OutputDebugStringW(ss.str().c_str());
			return;
		}
		else {
			std::wstringstream ss;
			ss << L"Assembly file found: " << assemblyFullPath << std::endl;
			OutputDebugStringW(ss.str().c_str());
		}

        if (!std::filesystem::exists(runtimeConfigFullPath)) {
            std::wstringstream ss;
            ss << L"Runtime config file not found: " << runtimeConfigFullPath << std::endl;
            OutputDebugStringW(ss.str().c_str());
            return;
        }
        else {
			std::wstringstream ss;
			ss << L"Runtime config file found: " << runtimeConfigFullPath << std::endl;
			OutputDebugStringW(ss.str().c_str());
        }

        if (!netRuntime->Initialize(runtimeConfigFullPath, assemblyFullPath)) {
            OutputDebugStringA("Failed to initialize .NET runtime\n");
            return;
        }

		OutputDebugStringA("Initialized .NET runtime\n");

        // Create game instance
        void* gameInstance = nullptr;
        if (!netRuntime->CreateInstance(L"LSHARP.Game", &gameInstance)) {
            OutputDebugStringA("Failed to create game instance\n");
            return;
        }

        // Load and call the Initialize method
        int result;
        if (!netRuntime->LoadAndCallMethod<int>(gameInstance, L"LSHARP.Game", L"Initialize", &result)) {
            OutputDebugStringA("Failed to call Initialize method\n");
            return;
        }
    }

    void Project::Run() {
        // Load and call the Start method
        int result;
        if (!netRuntime->LoadAndCallMethod<int>(gameInstance, L"LSHARP.Game", L"Start", &result)) {
            OutputDebugStringA("Failed to call Start method\n");
            return;
        }
    }

    void Project::Update() {
        int result;
        if (!netRuntime->LoadAndCallMethod<int>(gameInstance, L"LSHARP.Game", L"Update", &result)) {
            OutputDebugStringA("Failed to call Update method\n");
            return;
        }
    }

    void Project::Render() {
        int result;
        if (!netRuntime->LoadAndCallMethod<int>(gameInstance, L"LSHARP.Game", L"Render", &result)) {
            OutputDebugStringA("Failed to call Render method\n");
            return;
        }
    }

    void Project::Shutdown() {
        int result;
        if (!netRuntime->LoadAndCallMethod<int>(gameInstance, L"LSHARP.Game", L"Render", &result)) {
            OutputDebugStringA("Failed to call Render method\n");
            return;
        }

        // Shutdown the runtime
        if (netRuntime) {
            netRuntime->Shutdown();
            netRuntime.reset();
        }
    }

    // Member variable to store the game instance
    void* gameInstance = nullptr;
}
