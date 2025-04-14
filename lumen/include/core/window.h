#pragma once

#include <Windows.h>
#include <stdexcept>
#include <functional>

namespace core {
	class Window {
	public:
		~Window();

		bool Initialize();
		bool Alive() const {
			return IsWindow(hardware);
		}
		UINT GetMSG() const {
			return msg;
		}
		void ProcessMessage(std::function<int(MSG)> func) const;

		static bool CreateClass(WNDCLASSEX klass);
		static LRESULT CALLBACK WinProc(
			HWND hardware,
			UINT message,
			WPARAM wideParam,
			LPARAM longParam
		);
	private:
		HWND hardware = NULL;
		int width = 800;
		int height = 600;
		bool alive = true;
		UINT msg{};
	};
}