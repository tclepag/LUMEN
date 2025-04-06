#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

#include "NetRuntime.h"

namespace Engine {
	class Project {
	public:
		Project() = default;
		~Project() = default;
		void Init();
		void Run();
		void Update();
		void Render();
		void Shutdown();
	private:
		std::unique_ptr<NetRuntime> netRuntime;
	};
}