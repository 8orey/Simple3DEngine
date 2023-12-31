
#include <iostream>

#include "EngineCore/Application.hpp"
#include "EngineCore/Logs.hpp"
#include "EngineCore/Window.hpp"

namespace EngineCore {

	Application::Application() {
        LOG_INFO("Open Application");
    };

	Application::~Application() {
		LOG_INFO("Close Application");
	};

	int Application::start(uint32_t WINDOW_WIDTH, uint32_t WINDOW_HEIGHT, const char* title) {
		m_pWindow = std::make_unique<Window>(title, WINDOW_WIDTH, WINDOW_HEIGHT);


		m_event_dispatcher.add_event_listener<EventMouseMoved>(
			[](EventMouseMoved& event) {
				// LOG_INFO("[MouseMoved] Mouse moved to {0}x{1}", event.x, event.y);
			}
		);

		m_event_dispatcher.add_event_listener<EventWindowResize>(
			[](EventWindowResize& event) {
				// LOG_INFO("[WindowResized] Change size to {0}x{1}", event.w, event.h);
			}
		);

		m_event_dispatcher.add_event_listener<EventWindowClose>(
			[&](EventWindowClose& event) {
				m_bCloseWindow = true;
				LOG_INFO("[WindowClose] Goodbye!");
			}
		);

		m_pWindow->set_event_callback(
			[&](BaseEvent& event) {
				m_event_dispatcher.dispatch(event);
			}
		);

		while (!m_bCloseWindow) {
			m_pWindow->on_update();
			on_update();
		}
		m_pWindow = nullptr;
		return 0;
	};

} 