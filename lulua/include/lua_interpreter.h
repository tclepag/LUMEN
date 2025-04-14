#pragma once

#include "../framework.h"
#include "lua_table.h"
#include <sstream>

#ifdef LULUA_EXPORTS
#define LULUA_API __declspec(dllexport)
#else
#define LULUA_API __declspec(dllimport)
#endif

namespace lua {
	class LuaValue;
	class LuaScript;
	enum class LuaInterpContext {
		SERVER,
		SHARED,
		CLIENT // Only imports files with
	};

	class LuaInterpreter {
	public:
		LuaInterpreter(LuaInterpContext context);
		~LuaInterpreter();

		// Loads a lua file by it's path
		LuaScript openLua(const std::string& luaFilePath);

		// Returns the lua value at an index in the lua stack
		LuaValue getValue(int index = 0);

		// Gets a global value with name
		LuaValue getGlobal(const char* globalName);

		// Sets global value
		void setGlobal(const char* name);

		// Converts lua at index to lua value
		LuaValue lua_tovalue(int index);

		// Creates a reference
		int reference(int index, const std::wstring& source) {
			lua_pushvalue(l, index);
			int ref = luaL_ref(l, LUA_REGISTRYINDEX);

			std::wstringstream sstr{};
			sstr << L"CREATED REF: " << ref << " AT SOURCE: " << source << "\n";
			OutputDebugString(sstr.str().c_str());
			return ref;
		}

		// Removes a reference
		void dereference(int index, const std::wstring& source) {
			luaL_unref(l, LUA_REGISTRYINDEX, index);

			std::wstringstream sstr{};
			sstr << L"DELETED REF: " << index << " AT SOURCE: " << source << "\n";
			OutputDebugString(sstr.str().c_str());
		}

		// Returns the lua_State
		lua_State* getState() const {
			return l;
		}

		void push(int value) { lua_pushinteger(l, value); }
		void push(double value) { lua_pushnumber(l, value); }
		void push(const std::string& value) { lua_pushstring(l, value.c_str()); }
		void push(const char* value) { lua_pushstring(l, value); }
		void push(bool value) { lua_pushboolean(l, value); }
		void push(std::nullptr_t) { lua_pushnil(l); }
		void push(lua_CFunction func) { lua_pushcfunction(l, func); }

	private:
		lua_State* l;
		LuaInterpContext context;
	};
}