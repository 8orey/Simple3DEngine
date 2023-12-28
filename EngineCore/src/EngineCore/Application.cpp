
#include <iostream>

#include <GLFW/glfw3.h>

#include "EngineCore/Application.hpp"
#include "EngineCore/Logs.hpp"

namespace EngineCore {

	Application::Application() {
        LOG_INFO("Create Application");
    };

	Application::~Application() {};

	int Application::start(uint32_t WINDOW_WIDTH, uint32_t WINDOW_HEIGHT, const char* title) {
        GLFWwindow* window;

        /* Initialize the library */
        if (!glfwInit())
            return -1;

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title, NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            // glClear(GL_COLOR_BUFFER_BIT);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();

            on_update();
        }

        glfwTerminate();
        return 0;
	};

} 