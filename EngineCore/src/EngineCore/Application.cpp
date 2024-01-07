
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>

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
#include "Rendering/OpenGL/Texture2D.hpp"

#include "Rendering/OpenGL/Renderer_OpenGL.hpp"

#include "Modules/UIModule.hpp"


namespace EngineCore {

    // ------ TO DELETE ------ //
    GLfloat skybox_vertexes[]{
        1.f,  1.f,  1.f,   0.f,  1.f,
        1.f,  1.f, -1.f,   1.f, 1.f,
        1.f, -1.f,  1.f,   0.f,  0.f,
        1.f, -1.f, -1.f,   1.f, 0.f,

       -1.f,  1.f,  1.f,   0.f, 1.f,
       -1.f,  1.f, -1.f,   1.f, 1.f,
       -1.f, -1.f,  1.f,   0.f, 0.f,
       -1.f, -1.f, -1.f,   1.f, 0.f,
    };

    GLuint skybox_triangles[] = {
        0, 1, 2, 3, 2, 1,
        4, 5, 6, 7, 6, 5,
        0, 4, 6, 0, 2, 6,
        1, 5, 3, 3, 7, 5,
        3, 7, 2, 7, 6, 2,
        1, 5, 0, 5, 0, 4,
    };

    const char* vertex_shader =
        R"(#version 460
        layout(location = 0) in vec3 vertex_position;
        layout(location = 1) in vec2 texture_coord;
        
        uniform mat4 module_matrix;
        uniform mat4 view_projection_matrix;
        uniform float texture_frame; 

        out vec3 color;        
        out vec2 tex_pos;

        void main() {
           tex_pos = texture_coord + vec2(texture_frame / 100.f, texture_frame / 100.f);
           gl_Position = view_projection_matrix * module_matrix * vec4(vertex_position, 1.0);
        })";

    const char* fragment_shader =
        R"(#version 460
        
        in vec2 tex_pos;
    
        layout(binding = 0) uniform sampler2D ourTexture;        
               
        out vec4 frag_color;

        void main() {
                    
            frag_color = texture(ourTexture, tex_pos);

        })";

    std::unique_ptr<ShaderProgram> p_shader_program;
    std::unique_ptr<VertexBuffer> p_positions_colors_vbo;
    std::unique_ptr<IndexBuffer> p_index_buffer;
    std::unique_ptr<Texture2D> p_texture;
    std::unique_ptr<VertexArray> p_vao;

    float scale[3] = { 1.f, 1.f, 1.f };
    float r_X = 0.f;
    float r_Y = 0.f;
    float translate[3] = { 0.f, 0.f, 0.f };
    float m_background_color[4] = { 0.345f, 0.510f, 0.510f, 0.f };
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
        camera.set_viewport_size(WINDOW_WIDTH, WINDOW_HEIGHT);

        {

            m_event_dispatcher.add_event_listener<EventMouseMoved>(
                [](EventMouseMoved& event) {}
            );

            m_event_dispatcher.add_event_listener<EventWindowResize>(
                [&](EventWindowResize& event) {
                    camera.set_viewport_size(event.w, event.h);
                }
            );

            m_event_dispatcher.add_event_listener<EventWindowClose>(
                [&](EventWindowClose& event) {
                    close();
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

            m_event_dispatcher.add_event_listener<EventMouseButtonPressed>(
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
        }

        // ------ TO DELETE ------ //

        

        const uint32_t width_t = 1000, height_t = 1000, channels = 3, r = 25;

        auto* data = new unsigned char[width_t * height_t * channels] {};

        for (size_t i = 0; i < height_t * width_t * channels; ++i) {
            if ((i / channels % width_t <= width_t / 2) and (i / channels / width_t <= height_t / 2)) {
                data[i] = 255;
            }
            if ((i / channels % width_t >= width_t / 2) and (i / channels / width_t >= height_t / 2)) {
                data[i] = 255;
            }
        }

        p_texture = std::make_unique<Texture2D>(data, width_t, height_t);
        
        
        delete[] data;

        p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, fragment_shader);
        if (!p_shader_program->is_compiled()) {
            return false;
        }

        BufferLayout buf_lay_2v3{
            ShaderDataType::Float3,
            ShaderDataType::Float2,
        };

        p_positions_colors_vbo = std::make_unique<VertexBuffer>(skybox_vertexes, sizeof(skybox_vertexes), buf_lay_2v3);
        p_index_buffer = std::make_unique<IndexBuffer>(skybox_triangles, sizeof(skybox_triangles) / sizeof(GLuint));
        p_vao = std::make_unique<VertexArray>();

        p_vao->add_vertex_buffer(*p_positions_colors_vbo);
        p_vao->set_index_buffer(*p_index_buffer);

        // ------ TO DELETE ------ //
        Renderer_OpenGL::enable_depth_testing();

		while (!m_bCloseWindow) {
            // ------ TO DELETE ------ //

            Renderer_OpenGL::set_clear_color(m_background_color);
            Renderer_OpenGL::clear();

            p_shader_program->bind();
            glm::mat4 scale_matrix = glm::scale(glm::mat4(1.f), glm::vec3(scale[0], scale[1], scale[2]));
            glm::mat4 r_matrix_x = glm::rotate(glm::mat4(1.f), glm::radians(r_X), glm::vec3(1, 0, 0));
            glm::mat4 r_matrix_y = glm::rotate(glm::mat4(1.f), glm::radians(r_Y), glm::vec3(0, 1, 0));
            glm::mat4 translate_matrix = glm::translate(glm::mat4(1.f), glm::vec3(translate[0], translate[1], translate[2]));
            glm::mat4 module_matrix = translate_matrix * r_matrix_y * r_matrix_x * scale_matrix;

            std::array<std::array<float, 3>, 4> trans = {
                std::array<float, 3>{10, 6, -2},
                std::array<float, 3>{1, -7,  2},
                std::array<float, 3>{16, 16, 16},
                std::array<float, 3>{-2, 3, 5},
            };

            p_texture->bind(0);
            p_shader_program->set_mat4("module_matrix", module_matrix);
            p_shader_program->set_mat4("view_projection_matrix", camera.get_projection_matrix() * camera.get_view_matrix());
            static float frame = 0;
            static bool speed = false;
            p_shader_program->set_float("texture_frame", frame);
            frame += static_cast<float>(speed);

            Renderer_OpenGL::draw(*p_vao);

            for (auto tr : trans) {
                auto t_matrix = glm::translate(glm::mat4(1.f), glm::vec3(tr[0], tr[1], tr[2]));
                p_shader_program->set_mat4("module_matrix", t_matrix);
                Renderer_OpenGL::draw(*p_vao);
            }

            UIModule::UI_draw_begin();
            
            on_UI_update();

            UIModule::UI_draw_end();

			m_pWindow->on_update();
			on_update();
		}
		m_pWindow = nullptr;
		return 0;
	};


    glm::vec2 Application::get_current_mouse_position() const {
        return m_pWindow->get_current_cursor_pos();
    };

    void Application::close() {
        m_bCloseWindow = true;
    }


} 