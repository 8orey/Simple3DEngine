#pragma once 

#include <array>
#include <functional>

#include "Keys.hpp"

namespace EngineCore {

	enum class EventType {
		WindowResize,
		WindowClose,

		KeyPressed,
		KeyReleased,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,


		EventsCount
	};

	struct BaseEvent {
		virtual ~BaseEvent() = default;
		virtual EventType get_type() const = 0;
	};


	class EventDispatcher {
	public:
		template<typename EventType>
		void add_event_listener(std::function<void(EventType&)> callback) {
			auto baseCallback = [func = std::move(callback)](BaseEvent& event) {
				func(static_cast<EventType&>(event));
				};
			m_eventCallbacks[static_cast<size_t>(EventType::type)] = std::move(baseCallback);
		}

		void dispatch(BaseEvent& event) {
			auto& callback = m_eventCallbacks[static_cast<size_t>(event.get_type())];
			if (callback) {
				callback(event);
			}
		}

	private:
		std::array<std::function<void(BaseEvent&)>, static_cast<size_t>(EventType::EventsCount)> m_eventCallbacks;
	};



	struct EventMouseMoved: public BaseEvent {

		EventMouseMoved(const double new_x, const double new_y) : x(new_x), y(new_y) {}

		virtual EventType get_type() const override {
			return type;
		}

		double x, y;
		static const EventType type = EventType::MouseMoved;
	};

	struct EventWindowResize : public BaseEvent {

		EventWindowResize(const uint32_t new_w, const uint32_t new_h) : w(new_w), h(new_h) {}

		virtual EventType get_type() const override {
			return type;
		}

		uint32_t w, h;
		static const EventType type = EventType::WindowResize;
	};

	struct EventWindowClose : public BaseEvent {
		virtual EventType get_type() const override {
			return type;
		}

		static const EventType type = EventType::WindowClose;
	};

	struct EventKeyPressed : public BaseEvent {

		EventKeyPressed(const KeyCode key_code, const bool repeated) : key_code(key_code), repeated(repeated) {}

		virtual EventType get_type() const override {
			return type;
		}

		KeyCode key_code;
		bool repeated;
		static const EventType type = EventType::KeyPressed;
	};

	struct EventKeyReleased : public BaseEvent {

		EventKeyReleased(const KeyCode key_code) : key_code(key_code) {}

		virtual EventType get_type() const override {
			return type;
		}

		KeyCode key_code;
		static const EventType type = EventType::KeyReleased;
	};

	struct EventMouseButtonPressed : public BaseEvent {

		EventMouseButtonPressed(const MouseKeyCode key_code, const double x, const double y) : key_code(key_code), x(x), y(y) {}

		virtual EventType get_type() const override {
			return type;
		}

		MouseKeyCode key_code;
		double x, y;
		static const EventType type = EventType::MouseButtonPressed;
	};

	struct EventMouseButtonReleased : public BaseEvent {

		EventMouseButtonReleased(const MouseKeyCode key_code, const double x, const double y) : key_code(key_code), x(x), y(y) {}

		virtual EventType get_type() const override {
			return type;
		}

		MouseKeyCode key_code;
		double x, y;
		static const EventType type = EventType::MouseButtonReleased;
	};

}