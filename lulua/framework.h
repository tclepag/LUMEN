#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Include Lua headers
extern "C" {
#include "lua/include/lua.h"
#include "lua/include/lauxlib.h"
#include "lua/include/lualib.h"
}

// Only define these if they're not already defined
#ifndef lua_tostring
#define lua_tostring(L,i) lua_tolstring(L, (i), NULL)
#endif

#ifndef lua_tointeger
#define lua_tointeger(L,i) lua_tointegerx(L, (i), NULL)
#endif

#ifndef lua_tonumber
#define lua_tonumber(L,i) lua_tonumberx(L, (i), NULL)
#endif

#ifndef lua_pcall
#define lua_pcall(L,n,r,f) lua_pcallk(L, (n), (r), (f), 0, NULL)
#endif

#ifndef luaL_loadfile
#define luaL_loadfile(L,f) luaL_loadfilex(L, (f), NULL)
#endif