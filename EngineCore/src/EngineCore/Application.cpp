#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <format>

#include <string>
#include <string_view>
#include <format>

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
        -1.0f,  1.f, -1.f,    -1.f,  0.f,  0.f,     1.f, 0.f,              // 1
        -1.0f,  1.f,  1.f,    -1.f,  0.f,  0.f,     1.f, 1.f,              // 2
        -1.0f, -1.f,  1.f,    -1.f,  0.f,  0.f,     0.f, 1.f,              // 3

        // BACK                                  
         1.0f, -1.f, -1.f,     1.f,  0.f,  0.f,     1.f, 0.f,              // 4
         1.0f,  1.f, -1.f,     1.f,  0.f,  0.f,     0.f, 0.f,              // 5
         1.0f,  1.f,  1.f,     1.f,  0.f,  0.f,     0.f, 1.f,              // 6
         1.0f, -1.f,  1.f,     1.f,  0.f,  0.f,     1.f, 1.f,              // 7

         // RIGHT
         -1.0f,  1.f, -1.f,     0.f,  1.f,  0.f,     0.f, 0.f,              // 8
          1.0f,  1.f, -1.f,     0.f,  1.f,  0.f,     1.f, 0.f,              // 9
          1.0f,  1.f,  1.f,     0.f,  1.f,  0.f,     1.f, 1.f,              // 10
         -1.0f,  1.f,  1.f,     0.f,  1.f,  0.f,     0.f, 1.f,              // 11

         // LEFT
         -1.0f, -1.f, -1.f,     0.f, -1.f,  0.f,     1.f, 0.f,              // 12
          1.0f, -1.f, -1.f,     0.f, -1.f,  0.f,     0.f, 0.f,              // 13
          1.0f, -1.f,  1.f,     0.f, -1.f,  0.f,     0.f, 1.f,              // 14
         -1.0f, -1.f,  1.f,     0.f, -1.f,  0.f,     1.f, 1.f,              // 15

         // TOP
         -1.0f, -1.f,  1.f,     0.f,  0.f,  1.f,     0.f, 0.f,              // 16
         -1.0f,  1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 0.f,              // 17
          1.0f,  1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 1.f,              // 18
          1.0f, -1.f,  1.f,     0.f,  0.f,  1.f,     0.f, 1.f,              // 19

          // BOTTOM
          -1.0f, -1.f, -1.f,    0.f,  0.f, -1.f,     0.f, 1.f,              // 20
          -1.0f,  1.f, -1.f,    0.f,  0.f, -1.f,     1.f, 1.f,              // 21
           1.0f,  1.f, -1.f,    0.f,  0.f, -1.f,     1.f, 0.f,              // 22
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

    std::unique_ptr<ShaderProgram> p_csp;
    std::unique_ptr<ShaderProgram> p_light_shader_program;

    std::unique_ptr<VertexBuffer> p_vbo;
    std::unique_ptr<IndexBuffer> p_index_buffer;
    std::unique_ptr<Texture2D> p_texture_diffuse;
    std::unique_ptr<Texture2D> p_texture_specular;
    std::unique_ptr<VertexArray> p_vao;

    // ------ TO DELETE ------ //


	Application::Application() {
        light = {};
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
            int width_t, height_t, channels;
            
            auto* data_diffuse = read_image("D:\\programming\\Simple3DEngine\\EngineCore\\container2_diffuse.jpg", width_t, height_t, channels);
            p_texture_diffuse = std::make_unique<Texture2D>(data_diffuse, width_t, height_t);

            auto* data_specular = read_image("D:\\programming\\Simple3DEngine\\EngineCore\\container2_specular.jpg", width_t, height_t, channels);
            p_texture_specular = std::make_unique<Texture2D>(data_specular, width_t, height_t);

        }


        // Read, Compile, Link cube_shader_programm
        {
            cube_vertex_shader = read_file("D:\\programming\\Simple3DEngine\\EngineCore\\src\\EngineCore\\Shaders\\CubeVertexShader.glsl");
            cube_fragment_shader = read_file("D:\\programming\\Simple3DEngine\\EngineCore\\src\\EngineCore\\Shaders\\CubeFragmentShader.glsl");
            p_csp = std::make_unique<ShaderProgram>(cube_vertex_shader.c_str(), cube_fragment_shader.c_str());
            if (!p_csp->is_compiled()) {
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

            auto start = glfwGetTime();

            Renderer_OpenGL::set_clear_color(background_color);
            Renderer_OpenGL::clear();

            auto view_projection_matrix = camera.get_projection_matrix() * camera.get_view_matrix();
            auto view_matrix = camera.get_view_matrix();

            std::array<glm::vec3, 2> light_pos {
                glm::vec3(
                    2 * sin(2.f / 3.f * glfwGetTime()), 2 * cos(2.f/3.f * glfwGetTime()), 1
                ),

                glm::vec3(
                    2 * sin(3.14 + 2.f / 3.f * glfwGetTime()), 2 * cos(3.14 + 2.f / 3.f * glfwGetTime()), 1

                )
            };

            struct light_ {
                glm::vec3 color;
                glm::vec3 pos;
            };

            std::array <glm::vec3, 7> objs{
                glm::vec3{0, 0, 0},
                glm::vec3{-10, 6, 3},
                glm::vec3{-1, 3, 5},
                glm::vec3{0, -3, 7},
                glm::vec3{2, 3, 4},
                glm::vec3{-3, 2, -1},
                glm::vec3{5, 1, -12}
            };


            // Draw Light 
            {   
                p_light_shader_program->bind();

                // light.color[0] = abs(sin(glfwGetTime() * 0.1f));
                // light.color[1] = abs(sin(glfwGetTime() * 0.3f));
                // light.color[2] = abs(sin(glfwGetTime() * 0.2f));

                light.ambient = light.color * glm::vec3(0.05f);
                light.diffuse = light.color * glm::vec3(0.4f);
                light.specular = glm::vec3(light.color[0], light.color[1], light.color[2]);
                light.shininess = 32.f;

                p_light_shader_program->set_vec3("light_color", light.color);

                for (const auto& cur_pos : light_pos) {

                    const auto translate_matrix = glm::translate(glm::mat4(1.f), cur_pos);
                    const auto scale_matrix = glm::scale(glm::mat4(1.f), glm::vec3(0.05f, 0.05f, 0.05f));

                    p_light_shader_program->set_mat4("mvp_matrix", view_projection_matrix * translate_matrix * scale_matrix);

                    Renderer_OpenGL::draw(*p_vao);
                }   

            }

            // Draw Cubes;
            {
                p_csp->bind();

                p_texture_diffuse->bind(0);
                p_texture_specular->bind(1);

                
                int i = 0;
                for (const auto& cur_pos : light_pos) {
                    auto light_pos_eye = glm::vec3(view_matrix * glm::vec4(cur_pos, 1.0f));
                    p_csp->set_vec3(std::format("pointLights[{}].position_eye", i).c_str(), light_pos_eye);

                    p_csp->set_vec3(std::format("pointLights[{}].ambient", i).c_str(), light_intense * light.ambient);
                    p_csp->set_vec3(std::format("pointLights[{}].diffuse", i).c_str(), light_intense * light.diffuse);
                    p_csp->set_vec3(std::format("pointLights[{}].specular", i).c_str(), light_intense * light.specular);
                    p_csp->set_float(std::format("pointLights[{}].shininess", i).c_str(), light.shininess);

                    p_csp->set_float(std::format("pointLights[{}].constant", i).c_str(), 1.0f);
                    p_csp->set_float(std::format("pointLights[{}].linear", i).c_str(), 0.09f);
                    p_csp->set_float(std::format("pointLights[{}].quadro", i).c_str(), 0.032f);
                    p_csp->set_float(std::format("pointLights[{}].intensity", i).c_str(), light_intense);

                    i++;
                }
                
                const auto scale_matrix = glm::scale(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.5f));

                p_csp->set_int("material.ambient", 0);
                p_csp->set_int("material.diffuse", 0);
                p_csp->set_int("material.specular", 1);
                p_csp->set_float("material.shininess", 32.f);



                float angle = 0;
                for (const auto& object : objs) {
                    auto t_matrix = glm::translate(glm::mat4(1.f), object);
                    glm::mat4 r_matrix;
                    r_matrix = glm::rotate(glm::mat4(1.f), angle, glm::vec3(1, 0, 0));
                    r_matrix = glm::rotate(r_matrix, angle, glm::vec3(0, 1, 0));
                    r_matrix = glm::rotate(r_matrix, angle, glm::vec3(0, 0, 1));
                    auto module_matrix = t_matrix * r_matrix * scale_matrix;
                    angle += 15;
                    p_csp->set_mat4("module_view_matrix", camera.get_view_matrix() * module_matrix);
                    p_csp->set_mat4("mvp_matrix", view_projection_matrix * module_matrix);
                    p_csp->set_mat3("normal_matrix",  glm::transpose(glm::inverse(glm::mat3(camera.get_view_matrix()* module_matrix))));

                    Renderer_OpenGL::draw(*p_vao);
                }
            }
            
            UIModule::UI_draw_begin();
            
            on_UI_update();

            UIModule::UI_draw_end();

			m_pWindow->on_update();

			on_update();

            // LOG_INFO(1.f / (glfwGetTime() - start));

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