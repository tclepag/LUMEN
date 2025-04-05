#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Window.h"

// Helper function for getting Windows error messages
std::string GetLastErrorAsString() {
    DWORD errorCode = GetLastError();
    if (errorCode == 0) {
        return "No error";
    }

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL
    );

    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);

    return "Error (" + std::to_string(errorCode) + "): " + message;
}

namespace Engine {
    std::vector<WNDCLASS> Window::classes = {};

    Window::Window(std::wstring className, int nCmdShow)
        : nCmdShow(nCmdShow), mode(WindowMode::Windowed), style(0), exStyle(0), width(800), height(600), x(0), y(0), hWnd(nullptr) {
        hInstance = GetModuleHandle(nullptr);
        SetWindowMode(mode);
        MakeWindow(className);
    }

    Window::~Window() {
        DestroyWindowHandle();
    }

    void Window::CreateClass(WNDCLASS wndClass) {
        if (!RegisterClass(&wndClass)) {
            throw std::runtime_error("WINDOW CLASS ERR: " + GetLastErrorAsString());
        }
        classes.push_back(wndClass);
    }

    WNDCLASS Window::GetClass(std::wstring className) {
        for (const auto& cls : classes) {
            if (wcscmp(cls.lpszClassName, className.c_str()) == 0) {
                return cls;
            }
        }
        throw std::runtime_error("Class not found: " + std::string(className.begin(), className.end()));
    }

    void Window::DestroyClass(std::wstring className) {
        UnregisterClass(className.c_str(), GetModuleHandle(nullptr));
    }

    void Window::Show() const {
        if (hWnd) {
            ShowWindow(hWnd, nCmdShow);
        }
    }

    void Window::Hide() const {
        if (hWnd) {
            ShowWindow(hWnd, SW_HIDE);
        }
    }

    void Window::Update() {
        UpdateWindowPos();
    }

    void Window::Resize(int width, int height) {
        SetWindowSize(width, height);
        UpdateWindowPos();
    }

    void Window::Move(int x, int y) {
        SetWindowPosition(x, y);
        UpdateWindowPos();
    }

    void Window::SetTitle(const wchar_t* title) {
        if (hWnd) {
            SetWindowText(hWnd, title);
        }
    }

    void Window::SetIcon(HICON hIcon) {
        if (hWnd) {
            SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        }
    }

    void Window::SetCursor(HCURSOR hCursor) {
        if (hWnd) {
            SetClassLongPtr(hWnd, GCLP_HCURSOR, (LONG_PTR)hCursor);
        }
    }

    void Window::SetWindowMenu(HMENU hMenu) {
        if (hWnd) {
            ::SetMenu(hWnd, hMenu);
        }
    }

    void Window::SetStyle(DWORD style) {
        SetWindowStyle(style);
        UpdateWindowPos();
    }

    void Window::SetPosition(int x, int y) {
        SetWindowPosition(x, y);
        UpdateWindowPos();
    }

    void Window::SetSize(int width, int height) {
        SetWindowSize(width, height);
        UpdateWindowPos();
    }

    void Window::SetVisible(bool visible) {
        if (hWnd) {
            ShowWindow(hWnd, visible ? SW_SHOW : SW_HIDE);
        }
    }

    void Window::SetMode(WindowMode mode) {
        SetWindowMode(mode);
        UpdateWindowPos();
    }

	void Window::ProcessMessages() {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {  // Use PeekMessage instead
            if (msg.message == WM_QUIT) {
                PostQuitMessage(0);  // Re-post to ensure proper shutdown
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
	}

    int Window::AddMessageHook(UINT msg, std::function<int(HWND, UINT, WPARAM, LPARAM)> hook) {
        int hookId = nextHookId++;
        hooks[msg].emplace_back(hookId, hook);
        return hookId;
    }

    void Window::RemoveMessageHook(UINT msg, int hookId) {
        if (hooks.find(msg) != hooks.end()) {
            auto& hookList = hooks[msg];
            hookList.erase(std::remove_if(hookList.begin(), hookList.end(),
                [hookId](const Hook& hook) { return hook.getId() == hookId; }), hookList.end());
            if (hookList.empty()) {
                hooks.erase(msg);
            }
        }
    }

    void Window::MakeWindow(std::wstring className) {
        // Create the window
        hWnd = CreateWindowEx(
            exStyle,                // Extended style
            className.c_str(),      // Class name
            L"LUMEN",               // Window title
            style,                  // Style
            x, y,                   // Position
            width, height,          // Size
            nullptr,                // Parent window
            nullptr,                // Menu
            hInstance,              // Instance handle
            this                    // Creation parameters
        );

        if (!hWnd) {
            std::string errorMsg = "CreateWindowEx failed: " + GetLastErrorAsString();
            OutputDebugStringA(errorMsg.c_str());
            throw std::runtime_error(errorMsg);
        }
    }

    void Window::DestroyWindowHandle() {
        if (hWnd) {
            ::DestroyWindow(hWnd);
            hWnd = nullptr;
        }
    }

    void Window::UpdateWindowPos() {
        if (!hWnd) {
            OutputDebugStringA("UpdateWindowPos: Invalid hWnd");
            return;
        }

        RECT rect = { 0, 0, width, height };
        AdjustWindowRectEx(&rect, style, FALSE, exStyle);
        int adjustedWidth = rect.right - rect.left;
        int adjustedHeight = rect.bottom - rect.top;

        if (!SetWindowPos(hWnd, nullptr, x, y, adjustedWidth, adjustedHeight, SWP_NOZORDER | SWP_NOACTIVATE)) {
            std::string errorMsg = "SetWindowPos failed: " + GetLastErrorAsString();
            OutputDebugStringA(errorMsg.c_str());
        }

        if (!InvalidateRect(hWnd, nullptr, TRUE)) {
            std::string errorMsg = "InvalidateRect failed: " + GetLastErrorAsString();
            OutputDebugStringA(errorMsg.c_str());
        }

        if (!::UpdateWindow(hWnd)) {
            std::string errorMsg = "::UpdateWindow failed: " + GetLastErrorAsString();
            OutputDebugStringA(errorMsg.c_str());
        }
    }

    void Window::SetWindowMode(WindowMode mode) {
        this->mode = mode;
        switch (mode) {
        case WindowMode::Fullscreen:
            style = WS_POPUP;
            exStyle = WS_EX_TOPMOST;
            break;
        case WindowMode::Windowed:
            style = WS_OVERLAPPEDWINDOW;
            exStyle = 0;
            break;
        case WindowMode::Borderless:
            style = WS_POPUP;
            exStyle = 0;
            break;
        }

        // Update the style if window already exists
        if (hWnd) {
            SetWindowLongPtr(hWnd, GWL_STYLE, style);
            SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);

            // When changing window styles, we need to update the window
            UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED;
            SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, flags);
        }
    }

    void Window::SetWindowStyle(DWORD style) {
        this->style = style;
        if (hWnd) {
            SetWindowLongPtr(hWnd, GWL_STYLE, style);
        }
    }

    void Window::SetWindowExStyle(DWORD exStyle) {
        this->exStyle = exStyle;
        if (hWnd) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);
        }
    }

    void Window::SetWindowPosition(int x, int y) {
        this->x = x;
        this->y = y;
    }

    void Window::SetWindowSize(int width, int height) {
        this->width = width;
        this->height = height;
    }

    LRESULT CALLBACK Window::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        // Get the Window instance from window user data
        Window* pWindow = nullptr;

        if (msg == WM_NCCREATE) {
            // Store the Window pointer passed in CreateWindowEx
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
        }
        else {
            // Retrieve the stored Window pointer
            pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        // Process message hooks if Window instance exists
        if (pWindow) {
            auto it = pWindow->hooks.find(msg);
            if (it != pWindow->hooks.end()) {
                for (const auto& hook : it->second) {
                    if (hook.getFunc()(hWnd, msg, wParam, lParam) != 0) {
                        return 0;
                    }
                }
            }
        }

        // Default message handling
        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }
}