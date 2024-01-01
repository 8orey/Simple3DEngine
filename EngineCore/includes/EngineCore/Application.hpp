#pragma once 

#include "EngineCore/Event.hpp"

#include <memory>

namespace EngineCore {

	class Application {
	public:
		
		Application();

		virtual ~Application();

		Application(const Application&) = delete;

		Application(Application&&) = delete;

		Application& operator=(const Application&) = delete;

		Application& operator=(Application&&) = delete;

		virtual int start(uint32_t WINDOW_WIDTH, uint32_t WINDOW_HEIGHT, const char* title);

		virtual void on_update() {};

	private:

		std::unique_ptr<class Window> m_pWindow;
		EventDispatcher m_event_dispatcher;
		bool m_bCloseWindow = false;

	};

}