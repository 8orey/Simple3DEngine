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

		void set_title(const char* title) const;

		void close();

		Camera camera;
		
		float m_background_color[4];

		struct PointLight {
			glm::vec3 position = glm::vec3(0.f);

			glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
			glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
			float shininess = 32.0f;

			float linear = 0.19f;
			float quadro = 0.05f;
			float intensity = 12.0f;
		};

		std::vector<PointLight> point_lights;

	private:

		std::unique_ptr<class Window> m_pWindow;
		EventDispatcher m_event_dispatcher;
		bool m_bCloseWindow = false;

	};

}