#pragma once

#include "Window.h"
#include "Graphics/Renderer.h"

namespace Engine {
    class WindowManager {
    private:
        struct WindowData {
            Window* window;
            Graphics::Renderer* renderer;
            // Could add additional window-specific components here
        };

        static std::unordered_map<uint32_t, WindowData> s_WindowRegistry;
        static uint32_t s_NextWindowId;
        static uint32_t s_MainWindowId;

    public:
        // Register a window with its renderer
        static uint32_t RegisterWindow(Window* window, Graphics::Renderer* renderer);

        // Get window by ID
        static Window* GetWindow(uint32_t id);

        // Get renderer for a window
        static Graphics::Renderer* GetRenderer(uint32_t windowId);

        // Unregister a window and its renderer
        static void UnregisterWindow(uint32_t id);

        // Set/get main window
        static void SetMainWindow(uint32_t id);
        static uint32_t GetMainWindowId() { return s_MainWindowId; }
        static Window* GetMainWindow() { return GetWindow(s_MainWindowId); }
        static Graphics::Renderer* GetMainRenderer() { return GetRenderer(s_MainWindowId); }
    };
}
