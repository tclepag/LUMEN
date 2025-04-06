#include "Project.h"
#include <iostream>

namespace Engine {
	Project::Project() : netRuntime(std::make_unique<NetRuntime>()) {
	}
	void Project::Init() {
		// Initialize the .NET runtime
		std::wstring assemblyPath = L"LumenLogic.dll";
		std::wstring runtimeConfigPath = L"LumenLogic.runtimeconfig.json";

		if (!netRuntime->Initialize(runtimeConfigPath, assemblyPath)) {
			std::cerr << "Failed to initialize .NET runtime" << std::endl;
			return;
		}
	}
	void Project::Run() {
		
	}
	void Project::Update() {
		
	}
	void Project::Render() {
		
	}
	void Project::Shutdown() {
		if (netRuntime) {
			netRuntime->Shutdown();
			netRuntime.reset();
		}
	}
}