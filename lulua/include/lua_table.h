#pragma once

#include "../framework.h"
#include <string>
#include <stdexcept>

#ifdef LULUA_EXPORTS
#define LULUA_API __declspec(dllexport)
#else
#define LULUA_API __declspec(dllimport)
#endif

namespace lua {
	class LuaInterpreter;
	class LuaValue;
	class LuaTable {
	public:
		LuaTable(LuaInterpreter* luaState, int index);
		~LuaTable();

		LuaValue operator[](const std::string& key) const;
	private:
		LuaInterpreter* l;
		int ref;
	};
}