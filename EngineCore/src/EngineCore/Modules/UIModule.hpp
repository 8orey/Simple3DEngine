#pragma once 

struct GLFWwindow;

namespace EngineCore {

	class UIModule {
	public:
		static void on_window_create(GLFWwindow* pWindow);
		static void on_window_close();
		static void UI_draw_begin();
		static void UI_draw_end();
		static void ShowExampleAppDockSpace(bool* p_open);

	};

}