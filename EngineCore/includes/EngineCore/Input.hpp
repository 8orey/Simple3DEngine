#pragma once 

#include "Keys.hpp"

namespace EngineCore {
    using size_t = unsigned long long;

	class Input {
	public:
		static bool is_key_pressed(const KeyCode keey_code);
		static void PressKey(const KeyCode key_code);
		static void ReleaseKey(const KeyCode key_code);

		static bool is_mouse_key_pressed(const MouseKeyCode keey_code);
		static void PressMouseKey(const MouseKeyCode key_code);
		static void ReleaseMouseKey(const MouseKeyCode key_code);

	private:
		static bool m_keys_pressed[static_cast<size_t>(KeyCode::KEY_LAST) + 1];
		static bool m_mouse_keys_pressed[static_cast<size_t>(MouseKeyCode::MOUSE_BUTTON_LAST) + 1];
	};


}
