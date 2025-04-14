#pragma once

#include "../framework.h"
#include "lua_interpreter.h"
#include <stdexcept>
#include <iostream>

#ifdef LULUA_EXPORTS
#define LULUA_API __declspec(dllexport)
#else
#define LULUA_API __declspec(dllimport)
#endif

namespace lua {
	class LuaFunction {
	public:
        LuaFunction(LuaInterpreter* luaState, int index);
        ~LuaFunction();

        template<typename RESULT, typename ...Args>
        RESULT call(Args ...args)
        {

            lua_rawgeti(l->getState(), LUA_REGISTRYINDEX, ref-1);
            int type = lua_type(l->getState(), -1);
            OutputDebugString((L"Type at ref " + std::to_wstring(ref) + L": " + std::wstring(lua_typename(l->getState(), type), lua_typename(l->getState(), type) + strlen(lua_typename(l->getState(), type))) + L"\n").c_str());
            lua_pop(l->getState(), 1);


            lua_rawgeti(l->getState(), LUA_REGISTRYINDEX, ref); // Get lua function index by reference
            if (!lua_isfunction(l->getState(), -1)) {
                lua_pop(l->getState(), 1); // Clean up the stack

                std::wstringstream sstr{};
                sstr << "FUNCTION AT REF: " << ref << " IS NOT A FUNCTION\n";

                OutputDebugString(sstr.str().c_str());

                throw std::runtime_error("Value at ref is not a function.");
            }
            (l->push(args), ...); // Push all arguments into lua stack

            // Call function
            if (lua_pcall(l->getState(), sizeof...(args), 1, 0) != LUA_OK) {
                std::string error = lua_tostring(l->getState(), -1);
                lua_pop(l->getState(), 1);
                std::cerr << "Lua func error: " << error << std::endl;
                throw std::runtime_error("FAILED TO CALL FUNCTION: " + error);
            }

            // Get the return value
            if constexpr (std::is_same_v<RESULT, int>) {
                if (!lua_isinteger(l->getState(), -1)) {
                    lua_pop(l->getState(), 1); // Remove the value from the stack
                    throw std::runtime_error("Expected an integer return value.");
                }
                int result = lua_tointeger(l->getState(), -1);
                lua_pop(l->getState(), 1); // Remove the value from the stack
                return result;
            }
            else if constexpr (std::is_same_v<RESULT, double>) {
                if (!lua_isnumber(l->getState(), -1)) {
                    lua_pop(l->getState(), 1); // Remove the value from the stack
                    throw std::runtime_error("Expected a number return value.");
                }
                double result = lua_tonumber(l->getState(), -1);
                lua_pop(l->getState(), 1); // Remove the value from the stack
                return result;
            }
            else if constexpr (std::is_same_v<RESULT, std::string>) {
                if (!lua_isstring(l->getState(), -1)) {
                    lua_pop(l->getState(), 1); // Remove the value from the stack
                    throw std::runtime_error("Expected a string return value.");
                }
                std::string result = lua_tostring(l->getState(), -1);
                lua_pop(l->getState(), 1); // Remove the value from the stack
                return result;
            }
            else if constexpr (std::is_same_v<RESULT, bool>) {
                if (!lua_isboolean(l->getState(), -1)) {
                    lua_pop(l->getState(), 1); // Remove the value from the stack
                    throw std::runtime_error("Expected a boolean return value.");
                }
                bool result = lua_toboolean(l->getState(), -1);
                lua_pop(l->getState(), 1); // Remove the value from the stack
                return result;
            }
            else if constexpr (std::is_same_v<RESULT, std::nullptr_t>) {
                if (!lua_isnil(l->getState(), -1)) {
                    lua_pop(l->getState(), 1); // Remove the value from the stack
                    throw std::runtime_error("Expected a nil return value.");
                }
                lua_pop(l->getState(), 1); // Remove the value from the stack
                return nullptr;
            }
            else {
                lua_pop(l->getState(), 1); // Remove the value from the stack
                throw std::runtime_error("Unsupported return type.");
            }
        }

	private:
        LuaInterpreter* l;
		int ref;
	};
}