#pragma once

#include <functional>
#include <Windows.h>

namespace Engine {
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
}

