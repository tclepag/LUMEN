#pragma once

#include "lua_interpreter.h"
#include <unordered_map>
// TODO: Create implementation

namespace plugins {
	// The base interface for C++ lua functionality
	// This is just a base class which provides the lua states
	class LuaPlugin {
	public:
		// Called when the plugin is loaded into the engine
		virtual void Initialize(lua::LuaInterpreter* lua) = 0;
		// Called when the plugin is finished initializing
		virtual void Begin(lua::LuaInterpreter* lua) = 0;
		// Called when the engine's physics update
		virtual void Update(lua::LuaInterpreter* lua) {}
		// Called when the engine's renderer redraws
		virtual void Render(lua::LuaInterpreter* lua) {}
		// Called when the plugin is disabled
		virtual void Disabled(lua::LuaInterpreter* lua) {}
		// Called when the plugin is enabled
		virtual void Enabled(lua::LuaInterpreter* lua) {}
		// Called when the plugin is being removed from the engine
		// mainly called when the engine is closing
		virtual void Closing(lua::LuaInterpreter* lua) = 0;
	protected:
		// Returns if the interpreter has this plugin active
		bool IsInterpreterActive(lua::LuaInterpreter* lua);
	private:
		std::unordered_map<lua::LuaInterpreter*, bool> activeInterpreters;
	};
}