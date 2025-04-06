#include "Window.h"
#include "WindowManager.h"
#include "Graphics/Renderer.h"

using namespace Engine;
using namespace Graphics;

extern "C" {
	__declspec(dllexport) uint32_t Engine_CreateWindow() {
		Window* window = new Window(L"LUMENAPPWND", SW_SHOW);
		Renderer* renderer = new Renderer(window);
		if (!renderer->Initialize()) {
			MessageBoxA(nullptr, "Failed to initialize renderer", "Error", MB_ICONERROR);
			return;
		}
		renderer->Resize();
		return WindowManager::RegisterWindow(window, renderer);
	}

	__declspec(dllexport) uint32_t Engine_GetMainWindowId() {
		return WindowManager::GetMainWindowId();
	}
}