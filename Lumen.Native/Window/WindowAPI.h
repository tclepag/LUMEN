#pragma once

#include "Window.h"

#pragma pack(push, 8)
struct CWNINFO {
    LPCWSTR className;   // Window class name
    LPCWSTR windowName;  // Window title
    DWORD winStyle;      // Window style
    DWORD exStyle;       // Window extended styles
    int width;           // Window width (in pixels)
    int height;          // Window height (in pixels)
    int posX;            // Window X position (in pixels)
    int posY;            // Window Y position (in pixels)
    HWND parent;         // Window parent
    HMENU menu;          // Window menu
    int show;            // Window show behavior
};
#pragma pack(pop)

extern "C" {
	// Window main lifecycle methods
	__declspec(dllexport) Window* __cdecl Native_CreateWindow();
	__declspec(dllexport) void __cdecl Native_DestroyWindow(Window* window);

	// Window lifecycle methods
	__declspec(dllexport) bool __cdecl Native_BuildWindow(Window* window, const CWNINFO* windowInfo);
    __declspec(dllexport) bool __cdecl Native_DemolishWindow(Window* window);
    __declspec(dllexport) int __cdecl Native_ProcessWindowMessages(Window* window);
    __declspec(dllexport) bool __cdecl Native_CloseWindow(Window* window);
}