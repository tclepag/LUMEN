#pragma once

#include <Windows.h>
#include <WinUser.h>
#include <string>
#include <iostream>

using std::wstring;

struct WNCLASS {
	LPCWSTR className; // Window class name
	UINT style; // Window simple styles
	HICON icon; // Window icon preview
	HICON smallIcon; // Window small icon preview
	HCURSOR cursor = LoadCursor(nullptr, IDC_ARROW); // Window cursor
};

struct WNINFO {
	const LPCWSTR className; // Window class name
	wstring windowName = L"Window"; // Window title
	DWORD winStyle = WS_OVERLAPPEDWINDOW; // Window style
	DWORD exStyle; // Window extended styles
	int width = CW_USEDEFAULT; // Window width (in pixels)
	int height = CW_USEDEFAULT; // Window height (in pixels)
	int posX = CW_USEDEFAULT; // Window X position (in pixels)
	int posY = CW_USEDEFAULT; // Window Y position (in pixels)
	const HWND parent = nullptr; // Window parent
	const HMENU menu = nullptr; // Window menu
	int show = SW_SHOWNORMAL; // Window show behavior
};

class Window {
public:
	// Window helpers

	static wstring CreateWinClass(const WNCLASS& windowClass);
	static LRESULT CALLBACK WindowProcedure(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
	);

	// Window class lifecycle methods

	Window() = default;
	~Window();

	// Window lifecycle methods

	bool Fenestrate(WNINFO& windowInfo); // Provides a new window by class name
	void Demolish(); // Destroys window hardware and info
	void ProcessMessages(); // Processes any queued window events

	// Window methods

	void SetWindowName(const LPCWSTR& windowName); // Sets the window's title bar name
	void SetWindowSizeX(const int& x); // Sets the window's width (in pixels)
	void SetWindowSizeY(const int& y); // Sets the window's height (in pixels)
	void SetWindowPosX(const int& x); // Sets the window's left position (in pixels)
	void SetWindowPosY(const int& y); // Sets the window's top position (in pixels)
	void SetWindowCmd(const int& cmd); // Sets the window's show command

	// Window getters

	LPCWSTR GetWindowName() const { return win->windowName.c_str(); }
	const int GetWindowSizeX() const { return win->width; }
	const int GetWindowSizeY() const { return win->height; }
	const int GetWindowPosX() const { return win->posX; }
	const int GetWindowPosY() const { return win->posY; }
	const int GetWindowCmd() const { return win->show; }
	const UINT GetWindowMsgType() const { return msg.message; }
private:
	// Window core members

	WNINFO* win; // Window info
	HWND hWnd; // Window hardware
	MSG msg; // Window event message
};