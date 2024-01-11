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

		virtual void init() {};

		virtual int start(size_t WINDOW_WIDTH, size_t WINDOW_HEIGHT, const char* title);

		virtual void on_update() {};
		
		virtual void on_UI_update() {};

		virtual void on_mouse_key_activity(const MouseKeyCode key_code, const float x, const float y, const bool pressed) {};

		glm::vec2 get_current_mouse_position() const;

		void close();

		float light_pos[3]{ 0.f, 0.f, 0.f };
		float light_col[3]{ 1.f, 1.f, 1.f };
		float ambient_factor = 0.1f;
		float diffuse_factor = 0.1f;
		float specular_factor = 1.f;
	    float shine_factor = 32.f;

		Camera camera;
	private:

		std::unique_ptr<class Window> m_pWindow;
		EventDispatcher m_event_dispatcher;
		bool m_bCloseWindow = false;

	};

}