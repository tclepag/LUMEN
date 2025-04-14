#include "include/core/window.h"

namespace core {
	Window::~Window() {
		DestroyWindow(hardware);
	}

	bool Window::Initialize()
	{
		hardware = CreateWindowEx(
			0,
			L"CoreWindow",
			L"Window",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			800,
			600,
			nullptr,
			nullptr,
			GetModuleHandle(nullptr),
			this
		);
		if (!hardware) {
			return false;
		}
		ShowWindow(hardware, SW_SHOWDEFAULT);
		UpdateWindow(hardware);
		return true;
	}
	void Window::ProcessMessage(std::function<int(MSG)> func) const
	{
		if (!hardware) {
			return;
		}

		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			int r = func(msg);
			if (r == 0) {
				return;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	bool Window::CreateClass(WNDCLASSEX klass)
	{
		if (!RegisterClassEx(&klass)) {
			return false;
		}
		return true;
	}
	LRESULT Window::WinProc(HWND hardware, UINT message, WPARAM wideParam, LPARAM longParam)
	{
		Window* pThis = nullptr;
		if (message == WM_CREATE) {
			// Get creation info from wide param
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(longParam);
			// lpCreateParams should be set as this in window creation
			// Reinterpret it as window
			Window* window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
			// Set window long ptr so that we can reference the window at any time
			SetWindowLongPtr(hardware, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			pThis = window;
		}
		else {
			// GWLP_USERDATA should point to the window class
			// Reinterpret it as the window class
			pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hardware, GWLP_USERDATA));
		}

		if (pThis) {
			pThis->msg = message;
		}

		// Do default behavior 
		return DefWindowProc(hardware, message, wideParam, longParam);
	}
}
