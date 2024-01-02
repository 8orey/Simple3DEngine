
#include "EngineCore/Input.hpp" 

namespace EngineCore {
	bool Input::m_keys_pressed[static_cast<size_t>(KeyCode::KEY_LAST) + 1] = {};

	bool Input::is_key_pressed(const KeyCode key_code) {
		return m_keys_pressed[static_cast<size_t>(key_code)];
	}

	void Input::PressKey(const KeyCode key_code) {
		m_keys_pressed[static_cast<size_t>(key_code)] = true;
	}

	void Input::ReleaseKey(const KeyCode key_code) {
		m_keys_pressed[static_cast<size_t>(key_code)] = false;
	}

	bool Input::m_mouse_keys_pressed[static_cast<size_t>(MouseKeyCode::MOUSE_BUTTON_LAST) + 1] = {};

	bool Input::is_mouse_key_pressed(const MouseKeyCode key_code) {
		return m_mouse_keys_pressed[static_cast<size_t>(key_code)];
	}

	void Input::PressMouseKey(const MouseKeyCode key_code) {
		m_mouse_keys_pressed[static_cast<size_t>(key_code)] = true;
	}

	void Input::ReleaseMouseKey(const MouseKeyCode key_code) {
		m_mouse_keys_pressed[static_cast<size_t>(key_code)] = false;
	}

}