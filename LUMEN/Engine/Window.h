#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>

namespace Engine {
    enum class WindowMode {
        Fullscreen,
        Windowed,
        Borderless
    };

    class Hook {
    public:
        Hook(int id, std::function<int(HWND, UINT, WPARAM, LPARAM)> func)
            : id(id), func(func) {
        }

        int getId() const { return id; }
        std::function<int(HWND, UINT, WPARAM, LPARAM)> getFunc() const { return func; }

    private:
        int id;
        std::function<int(HWND, UINT, WPARAM, LPARAM)> func;
    };

    class Window {
    public:
        Window(std::wstring className, int nCmdShow);
        ~Window();

        // Public static methods
        static void CreateClass(WNDCLASS wndClass);
        static WNDCLASS GetClass(std::wstring className);
        static void DestroyClass(std::wstring className);
        static LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        // Public methods
        // Window management
        void Show() const;
        void Hide() const;
        void Update();
        void Resize(int width, int height);
        void Move(int x, int y);
        void SetTitle(const wchar_t* title);
        void SetIcon(HICON hIcon);
        void SetCursor(HCURSOR hCursor);
        void SetWindowMenu(HMENU hMenu); // Renamed to avoid collision
        void SetStyle(DWORD style);
        void SetPosition(int x, int y);
        void SetSize(int width, int height);
        void SetVisible(bool visible);
        void SetMode(WindowMode mode);
		void ProcessMessages();

        // Window messages
        int AddMessageHook(UINT msg, std::function<int(HWND, UINT, WPARAM, LPARAM)> hook);
        void RemoveMessageHook(UINT msg, int hookId);

        // Getters
        HWND GetHandle() const { return hWnd; }
        int GetWidth() const { return width; }
        int GetHeight() const { return height; }

    private:
        // Private static members
        static std::vector<WNDCLASS> classes;

        // Private members
        HWND hWnd;
        HINSTANCE hInstance;
        int nCmdShow;
        WindowMode mode;
        DWORD style;
        DWORD exStyle;
        int width;
        int height;
        int x;
        int y;

        std::unordered_map<UINT, std::vector<Hook>> hooks;
        int nextHookId = 1;

        // Private methods
        void MakeWindow(std::wstring className);
        void DestroyWindowHandle();
        void UpdateWindowPos();
        void SetWindowMode(WindowMode mode);
        void SetWindowStyle(DWORD style);
        void SetWindowExStyle(DWORD exStyle);
        void SetWindowPosition(int x, int y);
        void SetWindowSize(int width, int height);
    };
}