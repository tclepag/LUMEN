#pragma once
#include "NetRuntime.h"
#include <memory>
#include <string>

namespace Engine {
    class Project {
    public:
        Project();
        ~Project() = default;

        // Main lifecycle methods
        bool Init();
        bool Run();
        bool Update();
        bool Render();
        bool Shutdown();

        // Diagnostics
        bool DumpAssemblyInfo();

    private:
        std::unique_ptr<NetRuntime> netRuntime;
        void* gameInstance = nullptr;

        // Helper methods for .NET operations
        bool LoadAssembly(const std::wstring& assemblyPath, const std::wstring& configPath);
        bool CreateGameInstance();
        bool CallGameMethod(const std::wstring& methodName, int* result = nullptr);
    };
}
