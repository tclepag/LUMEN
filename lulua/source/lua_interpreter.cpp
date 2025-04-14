#include "pch.h"
#include "include/lua_interpreter.h"
#include "include/lua_script.h"
#include "include/lua_value.h"

namespace lua {
	LuaInterpreter::LuaInterpreter(LuaInterpContext context)
		: context(context)
	{
		l = luaL_newstate();
		luaL_openlibs(l);
	}
	LuaInterpreter::~LuaInterpreter()
	{
		if (l) {
			lua_close(l);
			l = nullptr;
		}
	}
	LuaScript LuaInterpreter::openLua(const std::string& luaFilePath)
	{
		return LuaScript(this, luaFilePath);
	}
	LuaValue LuaInterpreter::getValue(int index)
	{
		return lua_tovalue(index);
	}
	LuaValue LuaInterpreter::getGlobal(const char* globalName)
	{
		lua_getglobal(l, globalName);
		LuaValue value = lua_tovalue(-1);

		if (value.is<std::nullptr_t>()) {
			std::cerr << "Global variable '" << globalName << "' is not defined or is nil." << std::endl;
		}

		lua_pop(l, 1);
		return value;
	}
	void LuaInterpreter::setGlobal(const char* name)
	{
		lua_setglobal(l, name);
	}
	LuaValue LuaInterpreter::lua_tovalue(int index)
	{
		int type = lua_type(l, index);
		switch (type) {
		case LUA_TNUMBER:
			if (lua_isinteger(l, index)) {
				return LuaValue(lua_Integer(lua_tointeger(l, index)));
			}
			else {
				return LuaValue(lua_Number(lua_tonumber(l, index)));
			}
		case LUA_TSTRING:
			return LuaValue(std::string(lua_tostring(l, index)).c_str());
		case LUA_TBOOLEAN:
			return LuaValue(bool(lua_toboolean(l, index)));
		case LUA_TNIL:
			return LuaValue(nullptr);
		case LUA_TTABLE:
			return LuaValue(new LuaTable(this, index));
		case LUA_TFUNCTION:
			return LuaValue(new LuaFunction(this, index));
		default:
			throw std::runtime_error("Unsupported Lua type");
		}
	}
}
