#pragma once

#include "shared.h"
#include <string>

namespace core {
	struct Win {
		std::string className;
		std::string windowName;
		UINT width;
		UINT height;
	};

	class Window {
	public:
		~Window();
		bool Init(Win info);

		HWND GetHWND() const {
			return _hWnd;
		}

		static void NewClass(WNDCLASSEX wndClass);
	private:
		HWND _hWnd;

		static LRESULT CALLBACK WinProc(
			HWND hWnd,
			UINT msg,
			LPARAM lParam,
			WPARAM wParam
		);
	};
}