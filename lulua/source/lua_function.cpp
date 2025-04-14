#include "pch.h"
#include "include/lua_function.h"
#include "include/lua_interpreter.h"

namespace lua {
	LuaFunction::LuaFunction(LuaInterpreter* luaState, int index)
		: l(luaState)
	{
		if (!lua_isfunction(l->getState(), index)) {
			throw std::invalid_argument("The value at given index isn't a function!");
		}

		ref = l->reference(index, L"LuaFunction");
	}
	LuaFunction::~LuaFunction()
	{
		l->dereference(ref, L"LuaFunction");
	}
}