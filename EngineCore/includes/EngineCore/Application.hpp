#pragma once 

#include "glm/vec3.hpp"
#include "EngineCore/Event.hpp"
#include "EngineCore/Camera.hpp"

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
		
		virtual void on_UI_update() {};

		virtual void on_mouse_key_activity(const MouseKeyCode key_code, const double x, const double y, const bool pressed) {};

		glm::vec2 get_current_mouse_position() const;

		void close();

		Camera camera;
	private:

		std::unique_ptr<class Window> m_pWindow;
		EventDispatcher m_event_dispatcher;
		bool m_bCloseWindow = false;

	};

}