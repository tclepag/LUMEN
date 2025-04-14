#include "pch.h"
#include "include/lua_script.h"
#include "include/lua_interpreter.h"
#include "include/lua_value.h"

namespace lua {
    void LuaScript::load() {
        if (luaL_loadfile(l->getState(), path.c_str()) != LUA_OK) {
            throw std::runtime_error(lua_tostring(l->getState(), -1));
        }

        // Store ref so that when we execute, it'll execute this script
        scriptRef = l->reference(-1, L"LuaScript");
    }

    void LuaScript::execute() {
        lua_rawgeti(l->getState(), LUA_REGISTRYINDEX, scriptRef); // Get our script by reference
        // Call it now
        if (lua_pcall(l->getState(), 0, LUA_MULTRET, 0) != LUA_OK) {
            const char* err = lua_tostring(l->getState(), -1);
            throw std::runtime_error(err);
        }
    }
}