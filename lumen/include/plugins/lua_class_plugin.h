#pragma once

#include "lua_interpreter.h"
#include "lua_class.h"
#include <string>
#include <unordered_map>
#include <functional>

// TODO: Create implementation

namespace plugins {
	// The base plugin class that is used to create classes that are meant to be exposed in lua
	// If designing classes that need preformance, security, or are core features then it is necessary to use this
	class LuaClassPlugin {
	public:
		LuaClassPlugin(const std::string& className);
		virtual ~LuaClassPlugin();

		virtual bool Initialize(lua::LuaInterpreter* lua);
		virtual void Register(lua::LuaInterpreter* lua);
		virtual void Shutdown(lua::LuaInterpreter* lua);

		const std::string& GetName() const {
			return name;
		}
	protected:
		// Registers class with lua
		template <typename ClassType>
		void RegisterKlass(lua::LuaInterpreter* lua);

		// Creates an object with C++ from lua
		template <typename ClassType, typename... Args>
		static int CreateObject(lua::LuaInterpreter* lua);

		// Registers a class factory under name
		void RegisterKlassFactory(const std::string& className,
			std::function<LuaClass* (lua::LuaInterpreter* lua)> factory);

	private:
		std::string name;
		std::unordered_map<lua::LuaInterpreter*, bool> initalizedInterpreters;
		std::unordered_map < std::string, std::function<LuaClass* (lua::LuaInterpreter* lua)>> classFactories;
	};
}