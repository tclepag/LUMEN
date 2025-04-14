#include <Windows.h>
#include <sstream>
#include <iostream>
#include <string>
#include "include/core/window.h"

#include "lua_interpreter.h"
#include "lua_script.h"
#include "lua_value.h"

std::wstring runtimeConfigPath = L"myapp.runtimeconfig.json";
std::wstring assemblyPath = L"myapp.dll";

std::wstring GetMessageString(UINT msg) {
    switch (msg) {
    case WM_PAINT: return L"WM_PAINT";
    case WM_DESTROY: return L"WM_DESTROY";
    case WM_QUIT: return L"WM_QUIT";
    case WM_KEYDOWN: return L"WM_KEYDOWN";
    case WM_KEYUP: return L"WM_KEYUP";
        // Add more cases as needed
    default: return L"UNKNOWN_MESSAGE";
    }
}

int log(lua_State* l) {  
   int nargs = lua_gettop(l);  

   // Print a custom prefix  
   OutputDebugStringA("CUSTOM: ");  

   // Process and print all arguments  
   for (int i = 1; i <= nargs; i++) {  
       const char* str = lua_tostring(l, i);  
       if (str) {  
           OutputDebugStringA(str);  
           OutputDebugStringA(" ");  
       }  
   }  

   OutputDebugStringA("\n");  
   return 0;  
}

int WINAPI WinMain(
   _In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ LPSTR lpCmdLine,
   _In_ int nCmdShow
)
{
   // Create window class first
   WNDCLASSEX ex = {};
   ex.cbSize = sizeof(WNDCLASSEX);
   ex.hInstance = hInstance;
   ex.hCursor = LoadCursor(nullptr, IDC_ARROW);
   ex.lpfnWndProc = core::Window::WinProc;
   ex.lpszClassName = L"CoreWindow";
   if (!core::Window::CreateClass(ex)) {
       throw std::runtime_error("FAILED TO CREATE WINDOW CLASS");
   }

   core::Window* window = new core::Window();
   if (!window->Initialize()) {
       throw std::runtime_error("FAILED TO CREATE WINDOW");
   }

   // Create lua state
   lua::LuaInterpreter* lua = new lua::LuaInterpreter(lua::LuaInterpContext::SHARED);

   // Environment

   // Remove print and replace it with log
   // 1: remove print
   lua->push(nullptr);
   lua->setGlobal("print");
   // 2: create log global function
   lua->push(log);
   lua->setGlobal("log");

   lua_getglobal(lua->getState(), "package");
   lua_getfield(lua->getState(), -1, "path");
   std::string currentPath = lua_tostring(lua->getState(), -1);
   std::string newPath = currentPath + ";./content/lua/?.lua";
   lua_pop(lua->getState(), 1);
   lua_pushstring(lua->getState(), newPath.c_str());
   lua_setfield(lua->getState(), -2, "path");
   lua_pop(lua->getState(), 1);


   // Load lua file
   lua::LuaScript script = lua->openLua("content/lua/autorun/test_autorun.lua");
   // Load and execute it
   script.load();
   script.execute();

   std::cout << "Hello World!\n";

   while (window->Alive()) {
       window->ProcessMessage([window](MSG msg)
           {
               return 1;
           }
       );
   }

   delete window;

   std::cout << "Goodbye World!\n";
}