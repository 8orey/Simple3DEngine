#include <GLFW/glfw3.h>

#include "EngineCore/Window.hpp"
#include "EngineCore/Logs.hpp"

#include "Rendering/OpenGL/Renderer_OpenGL.hpp"

#include "Modules/UIModule.hpp"

namespace EngineCore {

    Window::Window(std::string title, const uint32_t width, const uint32_t height)
        : m_data({ move(title), width, height })
	{
		int exitCode = init();
	};
	
	Window::~Window() {
		shutdown();
	};

	int Window::init() {
        LOG_INFO("Window created: '{0}'. size {1}x{2}", m_data.title, m_data.width, m_data.height);

        glfwSetErrorCallback([](int error_code, const char* desc) {
            LOG_CRITICAL("GLFW error: {0}", desc); 
            }
        );

        if (!glfwInit()) {
            LOG_CRITICAL("Failed to initialize GLFW!");
            return -1;
        }

        m_pWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);

        if (!m_pWindow)
        {
            LOG_CRITICAL("Can't create a window: {0}", m_data.title);
            glfwTerminate();
            return -2;
        }

        if (!Renderer_OpenGL::init(m_pWindow)) {
            LOG_CRITICAL("Failed to initialize Renderer_OpenGL!");
            return -3;
        };

        glfwSetWindowUserPointer(m_pWindow, &m_data);

        glfwSetKeyCallback(m_pWindow, 
            [](GLFWwindow* pWindow, int key, int scancode, int action, int mods) {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));

                switch (action) {

                case GLFW_PRESS: {
                    EventKeyPressed event(static_cast<KeyCode>(key), false);
                    data.eventCallbackFn(event);
                    break;
                }
                case GLFW_RELEASE: {
                    EventKeyReleased event(static_cast<KeyCode>(key));
                    data.eventCallbackFn(event);
                    break;
                }
                case GLFW_REPEAT: {
                    EventKeyPressed event(static_cast<KeyCode>(key), true);
                    data.eventCallbackFn(event);
                    break; 
                    }
                }
            }
        );

        glfwSetMouseButtonCallback(m_pWindow,
            [](GLFWwindow* pWindow, int key, int action, int mods) {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                double x, y;
                glfwGetCursorPos(pWindow, &x, &y);
                
                switch (action) {
                case GLFW_PRESS: {
                    EventMouseButtonPressed event(static_cast<MouseKeyCode>(key), x, y);
                    data.eventCallbackFn(event);
                    break;
                }
                case GLFW_RELEASE: {
                    EventMouseButtonReleased event(static_cast<MouseKeyCode>(key), x, y);
                    data.eventCallbackFn(event);
                    break;
                }
                };
            }
        );

        glfwSetWindowSizeCallback(m_pWindow, 
            [](GLFWwindow* pWindow, int width, int height) {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                data.width = width;
                data.height = height;

                EventWindowResize event(width, height);
                data.eventCallbackFn(event);
            }
        );

        glfwSetCursorPosCallback(m_pWindow,
            [](GLFWwindow* pWindow, double x, double y){
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                
                EventMouseMoved event(x, y);
                data.eventCallbackFn(event);
            }
        );

        glfwSetWindowCloseCallback(m_pWindow, 
            [](GLFWwindow* pWindow) {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                
                EventWindowClose event;
                data.eventCallbackFn(event);
            }
        );

        glfwSetFramebufferSizeCallback(m_pWindow,
            [](GLFWwindow* pWindow, int width, int height) {
                Renderer_OpenGL::set_viewport(width, height);
            }
        );

        UIModule::on_window_create(m_pWindow);
        return 0;
	}

	void Window::shutdown() {
        UIModule::on_window_close();
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
	}

	void Window::on_update() {
        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
	}

    glm::vec2 Window::get_current_cursor_pos() const {
        double x, y;
        glfwGetCursorPos(m_pWindow, &x, &y);
        return glm::vec2(x, y);
    };

}