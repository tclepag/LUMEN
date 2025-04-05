#include <windows.h>
#include <string>
#include "Engine/Core.h"


// List of version-specific C++ redistributable DLLs
const std::wstring vcrdlls[] = {
    L"msvcp140.dll",
    L"vcruntime140.dll"
};

// Checks if the end-user has the version-specific C++ redistributables installed
static bool hasCPPRedistributables() {
    for (const auto& dll : vcrdlls) {
        HMODULE hMod = LoadLibrary(dll.c_str());
		if (!hMod) {
			// DLL not found, return false
			return false;
        }
        else {
			// DLL found, free the library
			FreeLibrary(hMod);
        }
    }
	// All DLLs found, return true
	return true;
}

int WINAPI WinMain(
    _In_ HINSTANCE hInstance, 
    _In_opt_ HINSTANCE hPrevInstance, 
    _In_ LPSTR lpCmdLine, 
    _In_ int nCmdShow) 
{
	// Check if the version-specific C++ redistributables are installed
	if (!hasCPPRedistributables()) {
		MessageBox(NULL, L"Please install the required C++ redistributables.", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

    // Create Core
    try {
        auto core = std::make_unique<Engine::Core>();
        if (!core->Init(hInstance)) {
            return -1;
        }
        // Run the core
        core->Run();
		// Shutdown the core
        core->Shutdown();
    }
    catch (const std::exception& e) {
        MessageBoxA(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        return -1;
    }
}
