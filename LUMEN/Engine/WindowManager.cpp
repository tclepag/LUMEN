#include "WindowManager.h"

namespace Engine {
    std::unordered_map<uint32_t, WindowManager::WindowData> WindowManager::s_WindowRegistry;
    uint32_t WindowManager::s_NextWindowId = 1;
    uint32_t WindowManager::s_MainWindowId = 0;

    uint32_t WindowManager::RegisterWindow(Window* window, Graphics::Renderer* renderer) {
        uint32_t id = s_NextWindowId++;

        WindowData data{};
        data.window = window;
        data.renderer = renderer;

        s_WindowRegistry[id] = data;

        // If this is the first window, make it the main window
        if (s_MainWindowId == 0) {
            s_MainWindowId = id;
        }

        return id;
    }

    Window* WindowManager::GetWindow(uint32_t id) {
        auto it = s_WindowRegistry.find(id);
        if (it != s_WindowRegistry.end()) {
            return it->second.window;
        }
        return nullptr;
    }

    Graphics::Renderer* WindowManager::GetRenderer(uint32_t windowId) {
        auto it = s_WindowRegistry.find(windowId);
        if (it != s_WindowRegistry.end()) {
            return it->second.renderer;
        }
        return nullptr;
    }

    void WindowManager::UnregisterWindow(uint32_t id) {
        s_WindowRegistry.erase(id);

        // If we removed the main window, choose another one if available
        if (id == s_MainWindowId) {
            if (!s_WindowRegistry.empty()) {
                s_MainWindowId = s_WindowRegistry.begin()->first;
            }
            else {
                s_MainWindowId = 0;
            }
        }
    }

    void WindowManager::SetMainWindow(uint32_t id) {
        if (s_WindowRegistry.find(id) != s_WindowRegistry.end()) {
            s_MainWindowId = id;
        }
    }
}