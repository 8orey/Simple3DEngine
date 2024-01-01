
#include <memory>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/backends/imgui_impl_glfw.h>

#include "EngineCore/Window.hpp"
#include "EngineCore/Logs.hpp"

#include "Rendering/OpenGL/ShaderProgram.hpp"
#include "Rendering/OpenGL/VertexBuffer.hpp"
#include "Rendering/OpenGL/VertexArray.hpp"
#include "Rendering/OpenGL/IndexBuffer.hpp"

namespace EngineCore {

    static bool s_GLFW_initialized = false;

    GLfloat positions_colors[]{
        0.5,  0.5, 0,  0.f, 1.f, 0.f,
       -0.5,  0.5, 0,  0.f, 0.f, 1.f,
        0.5, -0.5, 0,  1.f, 0.f, 0.f,
       -0.5, -0.5, 0,  0.f, 1.f, 0.f,
    };

    GLuint indicies[] = {
        0, 1, 3, 0, 2, 3,  
    };

    const char* vertex_shader =
        R"(#version 440
        layout(location = 0) in vec3 vertex_position;
        layout(location = 1) in vec3 vertex_color;
        uniform mat4 module_matrix;
        out vec3 color;
        void main() {
           color = vertex_color;
           gl_Position = module_matrix * vec4(vertex_position, 1.0);
        })";

    const char* fragment_shader =
        R"(#version 440
        in vec3 color;
        out vec4 frag_color;
        void main() {
           frag_color = vec4(color, 1.0);
        })";

    std::unique_ptr<ShaderProgram> p_shader_program;
    std::unique_ptr<VertexBuffer> p_positions_colors_vbo;
    std::unique_ptr<IndexBuffer> p_index_buffer;
    std::unique_ptr<VertexArray> p_vao;

    float scale[3] = { 1.f, 1.f, 1.f };
    float rotate = 0.f;
    float translate[3] = { 0.f, 0.f, 0.f };

    Window::Window(std::string title, const uint32_t width, const uint32_t height)
        : m_data({ move(title), width, height })
	{
		int exitCode = init();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);

	};
	
	Window::~Window() {
		shutdown();
	};

	int Window::init() {

        LOG_INFO("Window created: '{0}'. size {1}x{2}", m_data.title, m_data.width, m_data.height);

        if (!s_GLFW_initialized and !glfwInit()) {
            LOG_CRITICAL("Failed initialize GLFW");
            return -1;
        }
        s_GLFW_initialized = true;

        m_pWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);
        if (!m_pWindow)
        {
            LOG_CRITICAL("Can't create a window: {0}", m_data.title);
            glfwTerminate();
            return -2;
        }

        glfwMakeContextCurrent(m_pWindow);


        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            LOG_CRITICAL("GLAD doesn't initialized");
            return -3;
        }

        glfwSetWindowUserPointer(m_pWindow, &m_data);

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
                glViewport(0, 0, width, height);
            }
        );

        p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, fragment_shader);
        if (!p_shader_program->isCompiled()) {
            return false;
        }

        BufferLayout buf_lay_2v3{
            ShaderDataType::Float3,
            ShaderDataType::Float3
        };
       
        p_positions_colors_vbo = std::make_unique<VertexBuffer>(positions_colors, sizeof(positions_colors), buf_lay_2v3);
        p_index_buffer = std::make_unique<IndexBuffer>(indicies, sizeof(indicies) / sizeof(GLuint));
        p_vao = std::make_unique<VertexArray>();

        p_vao->add_vertex_buffer(*p_positions_colors_vbo);
        p_vao->set_index_buffer(*p_index_buffer);
        
        return 0;
	}

	void Window::shutdown() {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
	}

	void Window::on_update() {
        
        glClearColor(m_background_color[0], m_background_color[1], m_background_color[2], m_background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(get_width());
        io.DisplaySize.y = static_cast<float>(get_height());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("Background Color Window");
        ImGui::ColorEdit4("Background Color", m_background_color);
        ImGui::SliderFloat3("Scale", scale, -2.f, 2.f);
        ImGui::SliderFloat("Rotate Oxy", &rotate, 0, 360);
        ImGui::SliderFloat3("Translate", translate, -1.f, +1.f);

        p_shader_program->bind();
        glm::mat4 scale_matrix(
            scale[0], 0.f,      0.f, 0.f,
            0.f, scale[1],      0.f, 0.f,
            0.f,      0.f, scale[2], 0.f,
            0.f,      0.f,      0.f, 1.f);

        float rotate_rad = glm::radians(rotate);
        glm::mat4 rotate_matrix(
            cos(rotate_rad), sin(rotate_rad), 0, 0,
            -sin(rotate_rad), cos(rotate_rad), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
        glm::mat4 translate_matrix(
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            translate[0], translate[1], translate[2], 1
        );

        glm::mat4 module_matrix = translate_matrix * rotate_matrix * scale_matrix;

        p_shader_program->set_mat4("module_matrix", module_matrix);
        p_vao->bind();
        glDrawElements(GL_TRIANGLES, p_vao->get_indicies_count(), GL_UNSIGNED_INT, nullptr);
        
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
	}
}