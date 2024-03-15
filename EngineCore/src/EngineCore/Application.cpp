
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <ImGui/imgui.h>

#include <format>
#include <vector>
#include <deque>
#include <thread>

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
#include "Rendering/OpenGL/Mesh.hpp"

#include "Rendering/OpenGL/Renderer_OpenGL.hpp"

#include "Modules/UIModule.hpp"
#include "Modules/FileRead.hpp"


namespace EngineCore {

    std::unique_ptr<ShaderProgram> p_csp;
    std::unique_ptr<ShaderProgram> p_lsp;

	Application::Application() {
        LOG_INFO("Open Application");
    };

	Application::~Application() {
		LOG_INFO("Close Application");
	};

    std::deque<float> deq(100, 0);
    float sum = 0;

	int Application::start(size_t WINDOW_WIDTH, size_t WINDOW_HEIGHT, const char* title) {

        m_pWindow = std::make_unique<Window>(title, WINDOW_WIDTH, WINDOW_HEIGHT);
        camera.set_viewport_size(
            static_cast<float>(WINDOW_WIDTH), 
            static_cast<float>(WINDOW_HEIGHT)
        );

        std::shared_ptr<Mesh> p_cube;

        std::shared_ptr<ShaderProgram> p_csp;
        std::shared_ptr<ShaderProgram> p_lsp;

        std::vector<std::shared_ptr<Texture2D>> vTextures(3);

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
                    LOG_INFO("[WindowClose] Goodbye!");
                    close();
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
            auto diffuse_image = read_image("D:\\programming\\Simple3DEngine\\EngineCore\\container2_diffuse.jpg");
            auto specular_image = read_image("D:\\programming\\Simple3DEngine\\EngineCore\\container2_specular.jpg");

            vTextures[0] = std::make_shared<Texture2D>(diffuse_image, Texture2D::type::ambient);
            vTextures[1] = std::make_shared<Texture2D>(diffuse_image, Texture2D::type::diffuse);
            vTextures[2] = std::make_shared<Texture2D>(specular_image, Texture2D::type::specular);

        }

        // Read, Compile, Link cube_shader_programm
        {
            std::string cube_vertex_shader = read_file("D:\\programming\\Simple3DEngine\\EngineCore\\src\\EngineCore\\Shaders\\CubeVertexShader.glsl");
            std::string cube_fragment_shader = read_file("D:\\programming\\Simple3DEngine\\EngineCore\\src\\EngineCore\\Shaders\\CubeFragmentShader.glsl");
            p_csp = std::make_shared<ShaderProgram>(cube_vertex_shader.c_str(), cube_fragment_shader.c_str());
            if (!p_csp->is_compiled()) {
                LOG_ERROR("Failed to compile cube_shader_programm");
                return -1;
            }
        }

        // Read, Compile, Link light_shader_programm
        {
            std::string light_vertex_shader = read_file("D:\\programming\\Simple3DEngine\\EngineCore\\src\\EngineCore\\Shaders\\LightVertexShader.glsl");
            std::string light_fragment_shader = read_file("D:\\programming\\Simple3DEngine\\EngineCore\\src\\EngineCore\\Shaders\\LightFragmentShader.glsl");
            p_lsp = std::make_shared<ShaderProgram>(light_vertex_shader.c_str(), light_fragment_shader.c_str());
            if (!p_lsp->is_compiled()) {
                LOG_ERROR("light_shader_programm");
                return -1;
            }
        }
        
        // Creating Mesh
        {
            std::vector<Vertex> cube_vertexes = {
                //    position             normal            UV                  index

                // FRONT
                   Vertex{glm::vec3{-1.0f, -1.f, -1.f}, glm::vec3{-1.f,  0.f,  0.f},     glm::vec2{0.f, 0.f}},              // 0
                   Vertex{glm::vec3{-1.0f,  1.f, -1.f}, glm::vec3{-1.f,  0.f,  0.f},     glm::vec2{1.f, 0.f}},              // 1
                   Vertex{glm::vec3{-1.0f,  1.f,  1.f}, glm::vec3{-1.f,  0.f,  0.f},     glm::vec2{1.f, 1.f}},              // 2
                   Vertex{glm::vec3{-1.0f, -1.f,  1.f}, glm::vec3{-1.f,  0.f,  0.f},     glm::vec2{0.f, 1.f}},              // 3

                   // BACK                                  
                      Vertex{glm::vec3{1.0f, -1.f, -1.f},  glm::vec3{1.f,  0.f,  0.f},     glm::vec2{1.f, 0.f}},              // 4
                      Vertex{glm::vec3{1.0f,  1.f, -1.f},  glm::vec3{1.f,  0.f,  0.f},     glm::vec2{0.f, 0.f}},              // 5
                      Vertex{glm::vec3{1.0f,  1.f,  1.f},  glm::vec3{1.f,  0.f,  0.f},     glm::vec2{0.f, 1.f}},              // 6
                      Vertex{glm::vec3{1.0f, -1.f,  1.f},  glm::vec3{1.f,  0.f,  0.f},     glm::vec2{1.f, 1.f}},              // 7

                      // RIGHT
                        Vertex{glm::vec3{-1.0f,  1.f, -1.f}, glm::vec3{0.f,  1.f,  0.f},     glm::vec2{0.f, 0.f}},              // 8
                        Vertex{glm::vec3{1.0f,  1.f, -1.f},  glm::vec3{0.f,  1.f,  0.f},     glm::vec2{1.f, 0.f}},              // 9
                        Vertex{glm::vec3{ 1.0f,  1.f,  1.f}, glm::vec3{0.f,  1.f,  0.f},     glm::vec2{1.f, 1.f}},              // 10
                        Vertex{glm::vec3{-1.0f,  1.f,  1.f}, glm::vec3{0.f,  1.f,  0.f},     glm::vec2{0.f, 1.f}},              // 11

                        // LEFT
                          Vertex{glm::vec3{-1.0f, -1.f, -1.f}, glm::vec3{0.f, -1.f,  0.f},     glm::vec2{1.f, 0.f}},              // 12
                          Vertex{glm::vec3{1.0f, -1.f, -1.f},  glm::vec3{0.f, -1.f,  0.f},     glm::vec2{0.f, 0.f}},              // 13
                          Vertex{glm::vec3{ 1.0f, -1.f,  1.f}, glm::vec3{0.f, -1.f,  0.f},     glm::vec2{0.f, 1.f}},              // 14
                          Vertex{glm::vec3{-1.0f, -1.f,  1.f}, glm::vec3{0.f, -1.f,  0.f},     glm::vec2{1.f, 1.f}},              // 15

                          // TOP
                            Vertex{glm::vec3{-1.0f, -1.f,  1.f}, glm::vec3{0.f,  0.f,  1.f},     glm::vec2{0.f, 0.f}},              // 16
                            Vertex{glm::vec3{-1.0f,  1.f,  1.f}, glm::vec3{0.f,  0.f,  1.f},     glm::vec2{1.f, 0.f}},              // 17
                            Vertex{glm::vec3{1.0f,  1.f,  1.f},  glm::vec3{0.f,  0.f,  1.f},     glm::vec2{1.f, 1.f}},              // 18
                            Vertex{glm::vec3{1.0f, -1.f,  1.f},  glm::vec3{0.f,  0.f,  1.f},     glm::vec2{0.f, 1.f}},              // 19

                            // BOTTOM
                             Vertex{glm::vec3{-1.0f, -1.f, -1.f}, glm::vec3{0.f,  0.f, -1.f},     glm::vec2{0.f, 1.f}},              // 20
                             Vertex{glm::vec3{-1.0f, 1.f, -1.f}, glm::vec3{0.f,  0.f, -1.f},     glm::vec2{1.f, 1.f}},              // 21
                             Vertex{glm::vec3{1.0f,  1.f, -1.f}, glm::vec3{0.f,  0.f, -1.f},     glm::vec2{1.f, 0.f}},              // 22
                             Vertex{glm::vec3{1.0f, -1.f, -1.f}, glm::vec3{0.f,  0.f, -1.f},     glm::vec2{0.f, 0.f}},              // 23
            };

            std::vector<GLuint> triangles = {
                0,   1,  2,  2,  3,  0, // front
                4,   5,  6,  6,  7,  4, // back
                8,   9, 10, 10, 11,  8, // right
                12, 13, 14, 14, 15, 12, // left
                16, 17, 18, 18, 19, 16, // top
                20, 21, 22, 22, 23, 20  // bottom
            };

            BufferLayout layout = {
                ShaderDataType::Float3,
                ShaderDataType::Float3,
                ShaderDataType::Float2
            };

            p_cube = std::make_shared<Mesh>(
                cube_vertexes, 
                vTextures,
                triangles, 
                layout, 
                VertexBuffer::EUsage::Dynamic
            );
            
        }

        Renderer_OpenGL::enable_depth_testing();

        init();

		while (!m_bCloseWindow) {
            auto start = glfwGetTime();

            Renderer_OpenGL::set_clear_color(background_color);
            Renderer_OpenGL::clear();

            glm::mat4 view_projection_matrix = camera.get_projection_matrix() * camera.get_view_matrix();
            glm::mat4 view_matrix = camera.get_view_matrix();
            glm::vec3 light_col = light_color_global;
            std::vector<glm::vec3> cubes_pos(cnt_cubes);
            std::vector<glm::vec3> light_pos(cnt_lights);

            p_csp->bind();
            p_csp->set_int("cnt_pointLights", cnt_lights);

            // calc Mesh positions
            {
                for (int i = 0; i < cnt_cubes; ++i) {
                    float k = (i % 3) + 1;
                    k = 2;
                    if (i % 3 == 0) {
                        cubes_pos[i] = k * glm::vec3(
                            sin(2.f * 3.14f / static_cast<float>(cnt_cubes) * i + 2.f / 3.f * glfwGetTime()),
                            cos(2.f * 3.14f / static_cast<float>(cnt_cubes) * i + 2.f / 3.f * glfwGetTime()),
                            1 //cos(2.f * 3.14f / static_cast<float>(cnt_cubes) * i + 2.f / 2.f * glfwGetTime())
                        );
                    }
                    else if (i % 3 == 1) {
                        cubes_pos[i] = k * glm::vec3(
                            sin(2.f * 3.14f / static_cast<float>(cnt_cubes) * i + 2.f / 3.f * glfwGetTime()),
                            cos(2.f * 3.14f / static_cast<float>(cnt_cubes) * i + 2.f / 3.f * glfwGetTime()),
                            sin(2.f * 3.14f / static_cast<float>(cnt_cubes) * i + 2.f / 2.f * glfwGetTime())
                        );
                    }
                    else if (i % 3 == 2) {
                        cubes_pos[i] = k * glm::vec3(
                            sin(2.f * 3.14f / static_cast<float>(cnt_cubes) * i + 2.f / 3.f * glfwGetTime()),
                            sin(2.f * 3.14f / static_cast<float>(cnt_cubes) * i + 2.f / 3.f * glfwGetTime()),
                            cos(2.f * 3.14f / static_cast<float>(cnt_cubes) * i + 2.f / 2.f * glfwGetTime())
                        );
                    }
                }

                for (int i = 0; i < cnt_lights; ++i) {
                    float k = (i % 3) + 1.f;
                    k = 0;
                    if (i % 3 == 0) {
                        light_pos[i] = k * glm::vec3(
                            sin(2.f * 3.14f / static_cast<float>(cnt_lights) * i + 2.f / 3.f * glfwGetTime()),
                            cos(2.f * 3.14f / static_cast<float>(cnt_lights) * i + 2.f / 3.f * glfwGetTime()),
                            1 //cos(2.f * 3.14f / static_cast<float>(cnt_cubes) * i + 2.f / 2.f * glfwGetTime())
                        );
                    }
                    else if (i % 3 == 1) {
                        light_pos[i] = k * glm::vec3(
                            sin(2.f * 3.14f / static_cast<float>(cnt_lights) * i + 2.f / 3.f * glfwGetTime()),
                            cos(2.f * 3.14f / static_cast<float>(cnt_lights) * i + 2.f / 3.f * glfwGetTime()),
                            sin(2.f * 3.14f / static_cast<float>(cnt_lights) * i + 2.f / 2.f * glfwGetTime())
                        );
                    }
                    else if (i % 3 == 2) {
                        light_pos[i] = k * glm::vec3(
                            sin(2.f * 3.14f / static_cast<float>(cnt_lights) * i + 2.f / 3.f * glfwGetTime()),
                            sin(2.f * 3.14f / static_cast<float>(cnt_lights) * i + 2.f / 3.f * glfwGetTime()),
                            cos(2.f * 3.14f / static_cast<float>(cnt_lights) * i + 2.f / 2.f * glfwGetTime())
                        );
                    }
                }
            }

            // Draw Cubes & lights;
            {
                for (int i = 0; i < cnt_lights; ++i) {
                    auto light_pos_eye = glm::vec3(view_matrix * glm::vec4(light_pos[i], 1.0f));

                    p_csp->set_vec3(std::format("pointLights[{}].position_eye", i).c_str(), light_pos_eye);

                    p_csp->set_vec3(std::format("pointLights[{}].ambient", i).c_str(), glm::vec3(0.05f) * light_col);
                    p_csp->set_vec3(std::format("pointLights[{}].diffuse", i).c_str(), glm::vec3(0.4f) * light_col);
                    p_csp->set_vec3(std::format("pointLights[{}].specular", i).c_str(), light_col);
                    p_csp->set_float(std::format("pointLights[{}].shininess", i).c_str(), 32.f);

                    p_csp->set_float(std::format("pointLights[{}].constant", i).c_str(), 1.0f);
                    p_csp->set_float(std::format("pointLights[{}].linear", i).c_str(), 0.09f);
                    p_csp->set_float(std::format("pointLights[{}].quadro", i).c_str(), 0.032f);
                    p_csp->set_float(std::format("pointLights[{}].intensity", i).c_str(), light_intense);

                }

                const auto scale_matrix = glm::scale(glm::mat4(1.f), glm::vec3(0.1f, 0.1f, 0.1f));

                for (const auto& object : cubes_pos) {
                    auto t_matrix = glm::translate(glm::mat4(1.f), object);

                    auto module_matrix = t_matrix * scale_matrix;

                    auto mvp_matrix = view_projection_matrix * module_matrix;
                    auto module_view_matrix = camera.get_view_matrix() * module_matrix;
                    auto normal_matrix = glm::transpose(glm::inverse(glm::mat3(camera.get_view_matrix() * module_matrix)));

                    p_csp->bind();

                    p_csp->set_mat4("mvp_matrix", mvp_matrix);
                    p_csp->set_mat4("module_view_matrix", module_view_matrix);
                    p_csp->set_mat3("normal_matrix", normal_matrix);

                    p_csp->set_float("material.shininess", 32);

                    p_cube->draw(*p_csp);
                }
            }

            //Draw lights;
            {
                const auto scale_matrix = glm::scale(glm::mat4(1.f), glm::vec3(0.1f, 0.1f, 0.1f));

                for (const auto& object : light_pos) {
                    auto t_matrix = glm::translate(glm::mat4(1.f), object);

                    auto module_matrix = t_matrix * scale_matrix;

                    auto mvp_matrix = view_projection_matrix * module_matrix;

                    p_lsp->bind();

                    p_lsp->set_mat4("mvp_matrix", mvp_matrix);

                    p_lsp->set_vec3("light_color", light_col);

                    p_cube->draw(*p_lsp);
                }
            }

            /*

            UIModule::UI_draw_begin();
            
            on_UI_update();

            UIModule::UI_draw_end();

            */

			m_pWindow->on_update();

			on_update();

            // write FPS
            {
                float value = 1 / (glfwGetTime() - start);

                deq.push_back(value);
                sum += value;
                sum -= deq.front();
                deq.pop_front();

                glfwSetWindowTitle(m_pWindow->get_window_ptr(),
                    std::format("EngineEditor | FPS: {}",
                        static_cast<int>(sum / static_cast<float>(deq.size()))).c_str());
            }


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