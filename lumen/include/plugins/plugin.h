#pragma once

// TODO: Create implementation

namespace plugins {
	// The base interface for all game engine plugins
	// A plugin is designed to extend or provide functionality that the engine does not provide
	// All plugins are also compiled as DLLs to provide modularity
	class Plugin {
	public:
		// Called when the plugin is loaded into the engine
		virtual void Initialize() = 0;
		// Called when the plugin is finished initializing
		virtual void Begin() = 0;
		// Called when the engine's physics update
		virtual void Update() {}
		// Called when the engine's renderer redraws
		virtual void Render() {}
		// Called when the plugin is disabled
		virtual void Disabled() {}
		// Called when the plugin is enabled
		virtual void Enabled() {}
		// Called when the plugin is being removed from the engine
		// mainly called when the engine is closing
		virtual void Closing() = 0;
	private:
		bool initialized = false; // Plugin's initialization state
		bool active = false; // Plugin's processing state, if disabled then the plugin will not process updates or redraws
	};
}