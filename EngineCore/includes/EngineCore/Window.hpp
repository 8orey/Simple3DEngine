#pragma once 

#include "EngineCore/Event.hpp"
#include "glm/vec2.hpp"

#include <string>
#include <functional>

struct GLFWwindow;

namespace EngineCore {

	class Window {
	public:
		using EventCallbackFn = std::function<void(BaseEvent&)>;

		Window(std::string title, const uint32_t width, const uint32_t height);

		~Window();

		Window(const Window&) = delete;

		Window(Window&&) = delete;

		Window& operator=(const Window&) = delete;

		Window& operator=(Window&&) = delete;

		void on_update();

		uint32_t get_width() const {
			return m_data.width;
		};
		uint32_t get_height() const {
			return m_data.height;
		};

		void set_event_callback(const EventCallbackFn& callback) {
			m_data.eventCallbackFn = callback;
		}

		glm::vec2 get_current_cursor_pos() const;

	private:
		struct WindowData {
			std::string title;
			uint32_t width;
			uint32_t height;
			EventCallbackFn eventCallbackFn;
		};


		int init();
		void shutdown();

		GLFWwindow* m_pWindow = nullptr;
		WindowData m_data;

	};

}