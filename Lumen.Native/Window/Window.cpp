#include "pch.h"
#include "Window.h"

wstring Window::CreateWinClass(const WNCLASS& windowClass) {
	WNDCLASSEX win = {0};
	win.cbSize = sizeof(WNDCLASSEX);
	win.hInstance = GetModuleHandle(nullptr);
	win.lpszClassName = windowClass.className;
	win.lpfnWndProc = WindowProcedure;
	win.hCursor = windowClass.cursor;
	win.hIcon = windowClass.icon;
	win.hIconSm = windowClass.smallIcon;
	win.style = windowClass.style;
	return win.lpszClassName;
}

LRESULT Window::WindowProcedure(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	Window* pThis = nullptr;
	// If we are creating a window, we'll try to reinterpret the creation struct
	// to get our window class
	if (uMsg == WM_CREATE) {
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pThis = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		// Set the hardware userdata point to our window for further use
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else {
		// If our window message isn't WM_CREATE we try to get window class
		// Reinterpret cast to get our window class, only works if the class
		// was already initialized in window procedure
		pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	// If the pointer to a window class doesn't exist
	// just do default window procedure
	if (!pThis) {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	// Handle window procedures here
	switch (uMsg) {
		case WM_DESTROY: {
			pThis->Demolish();
			return 0; // We don't want windows to handle this event
		}
		// Add more messages here
	}

	// If we made it here, that means the event wasn't overriden
	// So we run the default window procedure for that event
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Window::~Window() {
	Demolish(); // Call demolish function
	// We already prevented double destruction so if this is called
	// after we demolished then nothing will happen
}

bool Window::Fenestrate(WNINFO& windowInfo) {
	// Retrieve window class via name
	WNDCLASSEX winClass = {};
	LPWNDCLASSEX lpwcx = &winClass;
	BOOL classExists = GetClassInfoEx(
		GetModuleHandle(nullptr),
		windowInfo.className,
		lpwcx
	);

	// If no window class was found, return false
	if (!classExists) {
		return false;
	}

	// Create window hardware
	hWnd = CreateWindowEx(
		windowInfo.exStyle,
		windowInfo.className,
		windowInfo.windowName.c_str(),
		windowInfo.winStyle,
		windowInfo.posX,
		windowInfo.posY,
		windowInfo.width,
		windowInfo.height,
		windowInfo.parent,
		windowInfo.menu,
		lpwcx->hInstance,
		this
	);

	// If the hardware failed to create, return false
	if (!hWnd) {
		return false;
	}

	// Set member window info to the create info
	win = &windowInfo;

	// We successfully initialized, return true
	return true;
}

void Window::Demolish() {
	if (hWnd) { DestroyWindow(hWnd); hWnd = nullptr; } // Delete hardware, only if it exists
	if (win) { win = nullptr; } // Delete window info, only if it exists
}

void Window::ProcessMessages() {
	MSG msg;
	while (GetMessage(&msg, hWnd, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Window::SetWindowName(const LPCWSTR& windowName) {
	if (!hWnd) {
		return;
	}

	// Set window name and update info value
	SetWindowText(hWnd, windowName);
	win->windowName = windowName;
}

void Window::SetWindowSizeX(const int& x) {
	if (!hWnd) {
		return;
	}

	// Set window size and update info value
	SetWindowPos(hWnd, nullptr, x, win->posY, win->width, win->height, SWP_NOZORDER);
	win->posX = x;
}

void Window::SetWindowSizeY(const int& y) {
	if (!hWnd) {
		return;
	}

	// Set window size and update info value
	SetWindowPos(hWnd, nullptr, win->posX, y, win->width, win->height, SWP_NOZORDER);
	win->posY = y;
}

void Window::SetWindowPosX(const int& x) {
	if (!hWnd) {
		return;
	}

	// Set window position and update info value
	SetWindowPos(hWnd, nullptr, win->posX, win->posY, x, win->height, SWP_NOZORDER);
	win->width = x;
}

void Window::SetWindowPosY(const int& y) {
	if (!hWnd) {
		return;
	}

	// Set window position and update info value
	SetWindowPos(hWnd, nullptr, win->posX, win->posY, win->width, y, SWP_NOZORDER);
	win->height = y;
}

void Window::SetWindowCmd(const int& cmd) {
	if (!hWnd) {
		return;
	}

	// Set window cmd and update info value
	ShowWindow(hWnd, cmd);
	win->show = cmd;
}