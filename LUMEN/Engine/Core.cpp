#include "Core.h"
#include "resource.h"

namespace Engine {
	Window* Core::GetWindow() const {
		return window.get();
	}

	bool Core::Init(HINSTANCE hInstance) {
		try {
			// Create and register window class
			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = Engine::Window::WinProc;
			wc.hInstance = hInstance;
			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wc.lpszClassName = L"LUMENAPPWND";

			Engine::Window::CreateClass(wc);

			// Create window
			window = std::make_unique<Window>(wc.lpszClassName, SW_SHOW);
			window->SetTitle(L"LUMEN");
			window->SetSize(1024, 768);
			window->Show();
			window->Update();
			return true;
		}
		catch (const std::exception& e) {
			MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR);
			return false;
		}

		// Initialization went well, return true
		return true;
	}

	void Core::Run() {
		window->AddMessageHook(WM_CLOSE, [this](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			active = false;
			return 0;
		});

		BeginMainLoop();
	}

	void Core::Update() {} // TODO: Implement update method
	void Core::Render() {}

	void Core::Shutdown() {
		// Set core state to inactive
		active = false;

		// Destroy window
		if (window) {
			window.reset();
		}
		// Unregister window class
		Window::DestroyClass(L"LUMENAPPWND");
	}

	void Core::BeginMainLoop() {
		while (active) {
			// Process messages
			window->ProcessMessages();

			Update(); // TODO: Implement a time class which calls this at a fixed rate
			Render();
		}
	}
}