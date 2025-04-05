#pragma once

#include "Window.h"
#include "Graphics/Renderer.h"
#include <memory>

namespace Engine {
	class Core {
	public:
		// Public getters
		Window* GetWindow() const;

		// Public methods
		bool Init(HINSTANCE hInstance);
		void Run();
		void Update();
		void Render();
		void Shutdown();
	private:
		// Private members
		std::unique_ptr<Window> window;
		std::unique_ptr<Graphics::Renderer> renderer;
		bool active = true;

		// Private methods
		void BeginMainLoop();
	};
}