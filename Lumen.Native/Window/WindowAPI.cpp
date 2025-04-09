#include "pch.h"
#include "WindowAPI.h"

extern "C" {
   __declspec(dllexport) Window* __cdecl Native_CreateWindow() {
       return new Window();
   }

   __declspec(dllexport) void __cdecl Native_DestroyWindow(Window* window) {
       delete window;
   }

   __declspec(dllexport) bool __cdecl Native_BuildWindow(Window* window, const CWNINFO* windowInfo) {
       // Setup C++ version of WNINFO
       WNINFO winInfo = {
           windowInfo->className,
           windowInfo->windowName,
           windowInfo->winStyle,
           windowInfo->exStyle,
           windowInfo->width,
           windowInfo->height,
           windowInfo->posX,
           windowInfo->posY,
           windowInfo->parent,
           windowInfo->menu,
           windowInfo->show
       };

       // Try building window
       try {
           return window->Fenestrate(winInfo);
       }
       catch (const std::exception& e) {
           // C++ error occurred, return false
           std::cerr << e.what() << std::endl;
           return false;
       }
   }

   __declspec(dllexport) bool __cdecl Native_DemolishWindow(Window* window) {
       // Try demolishing window
       try {
           window->Demolish();
           return true;
       }
       catch (const std::exception& e) {
           // C++ error occurred, return false
           std::cerr << e.what() << std::endl;
           return false;
       }
   }

   __declspec(dllexport) bool __cdecl Native_ProcessWindowMessages(Window* window) {
       // Try processing message
       try {
           window->ProcessMessages();
           return true;
       }
       catch (const std::exception& e) {
           // C++ error occurred, return false
           std::cerr << e.what() << std::endl;
           return false;
       }
   }
}