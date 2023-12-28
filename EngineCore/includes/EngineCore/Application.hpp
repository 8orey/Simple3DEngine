#pragma once 

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

	};

}