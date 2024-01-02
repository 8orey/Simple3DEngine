
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>

#include <ImGui/imgui.h>

#include "EngineCore/Application.hpp"
#include "EngineCore/Logs.hpp"
#include "EngineCore/Window.hpp"
#include "EngineCore/Camera.hpp"
#include "EngineCore/Input.hpp"

#include "Rendering/OpenGL/ShaderProgram.hpp"
#include "Rendering/OpenGL/VertexBuffer.hpp"
#include "Rendering/OpenGL/VertexArray.hpp"
#include "Rendering/OpenGL/IndexBuffer.hpp"

#include "Rendering/OpenGL/Renderer_OpenGL.hpp"

#include "Modules/UIModule.hpp"

namespace EngineCore {

    // ------ TO DELETE ------ //
    GLfloat positions_colors[]{
         1, 1, 1,   1.f, 1.f, 0.f,
        -1, 1, 1,   1.f, 1.f, 0.f,
         1,-1, 1,   1.f, 1.f, 0.f,
        -1,-1, 1,   1.f, 1.f, 0.f,

         1, 1,-1,   0.f, 1.f, 1.f,
        -1, 1,-1,   0.f, 1.f, 1.f,
         1,-1,-1,   0.f, 1.f, 1.f,
        -1,-1,-1,   0.f, 1.f, 1.f,
    };

    GLuint indicies[] = {
        0, 1, 3, 0, 2, 3,
        4, 7, 5, 4, 7, 6,

        1, 4, 0, 1, 4, 5,
        0, 6, 2, 0, 6, 4,
        2, 7, 3, 2, 7, 6,
        1, 7, 5, 1, 7, 3,
    };

    const char* vertex_shader =
        R"(#version 440
        layout(location = 0) in vec3 vertex_position;
        layout(location = 1) in vec3 vertex_color;
        uniform mat4 module_matrix;
        uniform mat4 view_projection_matrix;
        out vec3 color;
        void main() {
           color = vertex_color;
           gl_Position = view_projection_matrix * module_matrix * vec4(vertex_position, 1.0);
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
    float r_X = 0.f;
    float r_Y = 0.f;
    float translate[3] = { 0.f, 0.f, 0.f };
    float m_background_color[4] = { 0.33, 0.33, 0.33, 0 };
    float r_X_speed = 1;
    float r_Y_speed = 1;
    bool r_by_Oxy = false;
    bool r_by_Oyz = false;

    // ------ TO DELETE ------ //


	Application::Application() {
        LOG_INFO("Open Application");
    };

	Application::~Application() {
		LOG_INFO("Close Application");
	};

	int Application::start(uint32_t WINDOW_WIDTH, uint32_t WINDOW_HEIGHT, const char* title) {
		m_pWindow = std::make_unique<Window>(title, WINDOW_WIDTH, WINDOW_HEIGHT);


		m_event_dispatcher.add_event_listener<EventMouseMoved>(
			[](EventMouseMoved& event) {}
		);

		m_event_dispatcher.add_event_listener<EventWindowResize>(
			[](EventWindowResize& event) {}
		);

		m_event_dispatcher.add_event_listener<EventWindowClose>(
			[&](EventWindowClose& event) {
				m_bCloseWindow = true;
				LOG_INFO("[WindowClose] Goodbye!");
			}
		);

        m_event_dispatcher.add_event_listener<EventKeyPressed>(
            [](EventKeyPressed& event) {
                Input::PressKey(static_cast<KeyCode>(event.key_code));
            }
        );

        m_event_dispatcher.add_event_listener<EventKeyReleased>(
            [](EventKeyReleased& event) {
                Input::ReleaseKey(static_cast<KeyCode>(event.key_code));
            }
        );

        m_event_dispatcher.add_event_listener<EventMouseButtonPressed> (
            [&](EventMouseButtonPressed& event) {
                Input::PressMouseKey(static_cast<MouseKeyCode>(event.key_code));
                auto p = get_current_mouse_position();
                on_mouse_key_activity(static_cast<MouseKeyCode>(event.key_code), p.x, p.y, true);
            }
        );

        m_event_dispatcher.add_event_listener<EventMouseButtonReleased>(
            [&](EventMouseButtonReleased& event) {
                Input::ReleaseMouseKey(static_cast<MouseKeyCode>(event.key_code));
                auto p = get_current_mouse_position();
                on_mouse_key_activity(static_cast<MouseKeyCode>(event.key_code), p.x, p.y, false);
            }
        );

		m_pWindow->set_event_callback(
			[&](BaseEvent& event) {
				m_event_dispatcher.dispatch(event);
			}
		);

        // ------ TO DELETE ------ //

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

        // ------ TO DELETE ------ //


		while (!m_bCloseWindow) {
            // ------ TO DELETE ------ //

            Renderer_OpenGL::set_clear_color(m_background_color);
            Renderer_OpenGL::clear();

            r_X += r_X_speed * r_by_Oxy;
            r_Y += r_Y_speed * r_by_Oyz;
            if (r_X > 360)
                r_X -= 360;
            if (r_Y > 360)
                r_Y -= 360;

            p_shader_program->bind();
            glm::mat4 scale_matrix(
                scale[0], 0.f, 0.f, 0.f,
                0.f, scale[1], 0.f, 0.f,
                0.f, 0.f, scale[2], 0.f,
                0.f, 0.f, 0.f, 1.f);

            float r_X_rad = glm::radians(r_X);
            glm::mat4 r_matrix_x(
                cos(r_X_rad), sin(r_X_rad), 0, 0,
                -sin(r_X_rad), cos(r_X_rad), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            );

            float r_Y_rad = glm::radians(r_Y);
            glm::mat4 r_matrix_y(
                cos(r_Y_rad), 0, -sin(r_Y_rad), 0,
                0, 1, 0, 0,
                sin(r_Y_rad), 0, cos(r_Y_rad), 0,
                0, 0, 0, 1
            );

            glm::mat4 translate_matrix(
                1.f, 0.f, 0.f, 0.f,
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                translate[0], translate[1], translate[2], 1
            );

            glm::mat4 module_matrix = translate_matrix * r_matrix_y * r_matrix_x * scale_matrix;

            p_shader_program->set_mat4("module_matrix", module_matrix);
            p_shader_program->set_mat4("view_projection_matrix", camera.get_projection_matrix() * camera.get_view_matrix());

            Renderer_OpenGL::draw(*p_vao);

            bool param = true;
            UIModule::UI_draw_begin();

            UIModule::ShowExampleAppDockSpace(&param);

            ImGui::Begin("Background Color Window");
            ImGui::ColorEdit4("Background Color", m_background_color);
            ImGui::SliderFloat3("Scale", scale, -2.f, 2.f);
            ImGui::SliderFloat("Rotate Oxy", &r_X, -180, 180);
            ImGui::SliderFloat("Rotate Oyz", &r_Y, -180, 180);
            ImGui::SliderFloat3("Translate", translate, -10.f, +10.f);
            ImGui::Checkbox("Rotate by Oxy", &r_by_Oxy);
            ImGui::Checkbox("Rotate by Oyz", &r_by_Oyz);

            on_main_UI_update();

            ImGui::End();

            on_UI_update();

            UIModule::UI_draw_end();

            // ------ TO DELETE ------ //


			m_pWindow->on_update();
			on_update();
		}
		m_pWindow = nullptr;
		return 0;
	};


    glm::vec2 Application::get_current_mouse_position() const {
        return m_pWindow->get_current_cursor_pos();
    };

} 