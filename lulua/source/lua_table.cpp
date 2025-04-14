#include "pch.h"
#include "include/lua_table.h"
#include "include/lua_interpreter.h"
#include "include/lua_value.h"

namespace lua {
	LuaTable::LuaTable(LuaInterpreter* luaState, int index)
		: l(luaState) 
	{
		if (!lua_istable(l->getState(), index)) {
			// TODO: maybe print what type was given?
			throw std::invalid_argument("Expected lua table, got other type!");
		}

		ref = l->reference(index, L"LuaTable");
	}
	LuaTable::~LuaTable()
	{
		l->dereference(ref, L"LuaTable");
	}
	LuaValue LuaTable::operator[](const std::string& key) const
	{
		lua_rawgeti(l->getState(), LUA_REGISTRYINDEX, ref); // Gets the table by reference, and makes it current context
		lua_pushstring(l->getState(), key.c_str());
		lua_gettable(l->getState(), -2);
		LuaValue value = l->lua_tovalue(-1);
		return value;
	}
}