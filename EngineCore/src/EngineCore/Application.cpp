
#include <glad/glad.h>
#include <sys/timeb.h>

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
#include "Modules/FileRead.hpp"


namespace EngineCore {

    // ------ TO DELETE ------ //

    GLfloat skybox_vertexes[] = {
        //    position             normal            UV                  index

        // FRONT
        -1.0f, -1.f, -1.f,    -1.f,  0.f,  0.f,     0.f, 0.f,              // 0
        -1.0f,  1.f, -1.f,    -1.f,  0.f,  0.f,     2.f, 0.f,              // 1
        -1.0f,  1.f,  1.f,    -1.f,  0.f,  0.f,     2.f, 2.f,              // 2
        -1.0f, -1.f,  1.f,    -1.f,  0.f,  0.f,     0.f, 2.f,              // 3

        // BACK                                  
         1.0f, -1.f, -1.f,     1.f,  0.f,  0.f,     2.f, 0.f,              // 4
         1.0f,  1.f, -1.f,     1.f,  0.f,  0.f,     0.f, 0.f,              // 5
         1.0f,  1.f,  1.f,     1.f,  0.f,  0.f,     0.f, 2.f,              // 6
         1.0f, -1.f,  1.f,     1.f,  0.f,  0.f,     2.f, 2.f,              // 7

         // RIGHT
         -1.0f,  1.f, -1.f,     0.f,  1.f,  0.f,     0.f, 0.f,              // 8
          1.0f,  1.f, -1.f,     0.f,  1.f,  0.f,     2.f, 0.f,              // 9
          1.0f,  1.f,  1.f,     0.f,  1.f,  0.f,     2.f, 2.f,              // 10
         -1.0f,  1.f,  1.f,     0.f,  1.f,  0.f,     0.f, 2.f,              // 11

         // LEFT
         -1.0f, -1.f, -1.f,     0.f, -1.f,  0.f,     2.f, 0.f,              // 12
          1.0f, -1.f, -1.f,     0.f, -1.f,  0.f,     0.f, 0.f,              // 13
          1.0f, -1.f,  1.f,     0.f, -1.f,  0.f,     0.f, 2.f,              // 14
         -1.0f, -1.f,  1.f,     0.f, -1.f,  0.f,     2.f, 2.f,              // 15

         // TOP
         -1.0f, -1.f,  1.f,     0.f,  0.f,  1.f,     0.f, 0.f,              // 16
         -1.0f,  1.f,  1.f,     0.f,  0.f,  1.f,     2.f, 0.f,              // 17
          1.0f,  1.f,  1.f,     0.f,  0.f,  1.f,     2.f, 2.f,              // 18
          1.0f, -1.f,  1.f,     0.f,  0.f,  1.f,     0.f, 2.f,              // 19

          // BOTTOM
          -1.0f, -1.f, -1.f,    0.f,  0.f, -1.f,     0.f, 2.f,              // 20
          -1.0f,  1.f, -1.f,    0.f,  0.f, -1.f,     2.f, 2.f,              // 21
           1.0f,  1.f, -1.f,    0.f,  0.f, -1.f,     2.f, 0.f,              // 22
           1.0f, -1.f, -1.f,    0.f,  0.f, -1.f,     0.f, 0.f,              // 23
    };


    GLuint skybox_triangles[] = {
        0,   1,  2,  2,  3,  0, // front
        4,   5,  6,  6,  7,  4, // back
        8,   9, 10, 10, 11,  8, // right
        12, 13, 14, 14, 15, 12, // left
        16, 17, 18, 18, 19, 16, // top
        20, 21, 22, 22, 23, 20  // bottom
    };

    std::string cube_vertex_shader;
    std::string cube_fragment_shader;
    std::string light_vertex_shader;
    std::string light_fragment_shader;

    std::unique_ptr<ShaderProgram> p_cube_shader_program;
    std::unique_ptr<ShaderProgram> p_light_shader_program;

    std::unique_ptr<VertexBuffer> p_vbo;
    std::unique_ptr<IndexBuffer> p_index_buffer;
    std::unique_ptr<Texture2D> p_texture;
    std::unique_ptr<VertexArray> p_vao;

    // ------ TO DELETE ------ //

    float m_background_color[4] = { 0.345f, 0.510f, 0.510f, 0.f };

	Application::Application() {
        LOG_INFO("Open Application");
    };

	Application::~Application() {
		LOG_INFO("Close Application");
	};

	int Application::start(size_t WINDOW_WIDTH, size_t WINDOW_HEIGHT, const char* title) {
        
        m_pWindow = std::make_unique<Window>(title, WINDOW_WIDTH, WINDOW_HEIGHT);
        camera.set_viewport_size(
            static_cast<float>(WINDOW_WIDTH), 
            static_cast<float>(WINDOW_HEIGHT)
        );

        // Add Events
        {

            m_event_dispatcher.add_event_listener<EventMouseMoved>(
                [](EventMouseMoved& event) {}
            );

            m_event_dispatcher.add_event_listener<EventWindowResize>(
                [&](EventWindowResize& event) {
                    camera.set_viewport_size(
                        static_cast<float>(event.w), 
                        static_cast<float>(event.h)
                    );
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

        // Creating Texture        
        {
            const size_t width_t = 1000, height_t = 1000, channels = 3, r = 25;
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
        }

        // Read, Compile, Link cube_shader_programm
        {
            cube_vertex_shader = read_file("D:\\programming\\Simple3DEngine\\EngineCore\\src\\EngineCore\\Shaders\\CubeVertexShader.glsl");
            cube_fragment_shader = read_file("D:\\programming\\Simple3DEngine\\EngineCore\\src\\EngineCore\\Shaders\\CubeFragmentShader.glsl");
            p_cube_shader_program = std::make_unique<ShaderProgram>(cube_vertex_shader.c_str(), cube_fragment_shader.c_str());
            if (!p_cube_shader_program->is_compiled()) {
                return false;
            }
        }

        // Read, Compile, Link light_shader_programm
        {
            light_vertex_shader = read_file("D:\\programming\\Simple3DEngine\\EngineCore\\src\\EngineCore\\Shaders\\LightVertexShader.glsl");
            light_fragment_shader = read_file("D:\\programming\\Simple3DEngine\\EngineCore\\src\\EngineCore\\Shaders\\LightFragmentShader.glsl");
            p_light_shader_program = std::make_unique<ShaderProgram>(light_vertex_shader.c_str(), light_fragment_shader.c_str());
            if (!p_light_shader_program->is_compiled()) {
                return false;
            }
        }

        // Push base cube model to GPU
        {
            BufferLayout buf_lay_2v3{
                ShaderDataType::Float3,
                ShaderDataType::Float3,
                ShaderDataType::Float2,
            };

            p_vbo = std::make_unique<VertexBuffer>(skybox_vertexes, sizeof(skybox_vertexes), buf_lay_2v3);
            p_index_buffer = std::make_unique<IndexBuffer>(skybox_triangles, sizeof(skybox_triangles) / sizeof(GLuint));
            p_vao = std::make_unique<VertexArray>();

            p_vao->add_vertex_buffer(*p_vbo);
            p_vao->set_index_buffer(*p_index_buffer);
        }

        Renderer_OpenGL::enable_depth_testing();

        init();

		while (!m_bCloseWindow) {
            Renderer_OpenGL::set_clear_color(m_background_color);
            Renderer_OpenGL::clear();

            auto view_projection_matrix = camera.get_projection_matrix() * camera.get_view_matrix();
            auto view_matrix = camera.get_view_matrix();

            // Draw Cubes;
            {
                p_cube_shader_program->bind();

                const std::array<const std::array<float, 3>, 6> trans = {
                    std::array<float, 3>{1, 3, -1},
                    std::array<float, 3>{1, -5,  1},
                    std::array<float, 3>{5, 3, 5},
                    std::array<float, 3>{-1, 1, -4},
                    std::array<float, 3>{-1, 5, 4},
                    std::array<float, 3>{-2, 1, 7},
                };

                p_texture->bind(0);

                p_cube_shader_program->set_float("ambient_factor", ambient_factor);
                p_cube_shader_program->set_float("diffuse_factor", diffuse_factor);
                p_cube_shader_program->set_float("specular_factor", specular_factor);
                p_cube_shader_program->set_float("shine_factor", shine_factor);

                p_cube_shader_program->set_vec3("light_color", light_col[0], light_col[1], light_col[2]);

                auto light_pos_eye = view_matrix * glm::vec4(light_pos[0], light_pos[1], light_pos[2], 1.0f);

                p_cube_shader_program->set_vec3("light_pos_eye", light_pos_eye[0], light_pos_eye[1], light_pos_eye[2]);

                const auto scale_matrix = glm::scale(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.5f));

                for (const auto& tr : trans) {
                    auto t_matrix = glm::translate(glm::mat4(1.f), glm::vec3(tr[0], tr[1], tr[2]));
                    auto module_matrix = t_matrix * scale_matrix;
                    p_cube_shader_program->set_mat4("module_view_matrix", camera.get_view_matrix() * module_matrix);
                    p_cube_shader_program->set_mat4("mvp_matrix", view_projection_matrix * module_matrix);
                    p_cube_shader_program->set_mat3("normal_matrix",  glm::transpose(glm::inverse(glm::mat3(camera.get_view_matrix()* module_matrix))));
                    Renderer_OpenGL::draw(*p_vao);
                }
            }
            
            // Draw Light 
            {
                auto translate_matrix = glm::translate(glm::mat4(1.f),
                    glm::vec3(light_pos[0], light_pos[1], light_pos[2]));
                const auto scale_matrix = glm::scale(glm::mat4(1.f), glm::vec3(0.1f, 0.1f, 0.1f));

                p_light_shader_program->bind();
                p_light_shader_program->set_mat4("mvp_matrix", view_projection_matrix * translate_matrix * scale_matrix);
                p_light_shader_program->set_vec3("light_color", light_col[0], light_col[1], light_col[2]);

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