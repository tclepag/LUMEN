#pragma once

#include "lua_interpreter.h"
#include <string>
#include <functional>
#include <unordered_map>

namespace plugins {
	// The base interface for all lua/C++ classes
	// This is meant to be used with LuaClassPlugin
	class LuaClass {
	public:
		LuaClass(const std::string& className);
		virtual ~LuaClass();

		// Registers this class to lua
		virtual void RegisterWithLua(lua::LuaInterpreter* lua);

		// Get C++ class from lua userdata
		static LuaClass* CheckFromLua(lua::LuaInterpreter* lua, int index, const char* className);
	protected:
		// Registers a method with return type and arguments
		template <typename... Args>
		void RegisterMethod(const std::string& name,
			std::function<void(LuaClass*, Args...)> method);

		// Lua calls this function
		// Looks at the methods of the class name that lua wants
		// and runs the method under that class
		static int MethodDispatcher(lua::LuaInterpreter* lua);

		std::string name;
		std::unordered_map<std::string, std::function<int(LuaClass*, lua::LuaInterpreter*)>> methods;
	};
}