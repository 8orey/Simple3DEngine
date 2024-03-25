
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <ImGui/imgui.h>

#include <format>
#include <vector>
#include <deque>

#include "EngineCore/Application.hpp"
#include "EngineCore/Logs.hpp"
#include "EngineCore/Window.hpp"
#include "EngineCore/Camera.hpp"
#include "EngineCore/Input.hpp"
#include "EngineCore/Model.hpp"

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

        Renderer_OpenGL::enable_depth_testing();

        auto CVSP = PROJECT_SOURCE_DIR "EngineCore/src/EngineCore/Shaders/cube.vert";
        auto CFSP = PROJECT_SOURCE_DIR "EngineCore/src/EngineCore/Shaders/cube.frag";
        auto VSP = PROJECT_SOURCE_DIR "EngineCore/src/EngineCore/Shaders/nanosuit.vert";
        auto FSP = PROJECT_SOURCE_DIR "EngineCore/src/EngineCore/Shaders/nanosuit.frag";
        auto MOP = PROJECT_SOURCE_DIR "resources/nanosuit/nanosuit.obj";
        auto CMP = PROJECT_SOURCE_DIR "resources/cube/cube.stl";


        ShaderProgram NSP(VSP, FSP);
        ShaderProgram CSP(CVSP, CFSP);


        init();

        for (int i = 0; i < 1; ++i) {
            point_lights.push_back({});
        }

        struct Entity {
            Model model;
            glm::mat4 module;
        };
        
        Entity cube{
            {CMP},
            glm::mat4(1.f)
        };

        Entity soldier{
            {MOP},
            glm::mat4(1.f)
        };
        


        auto draw = [&](Entity* en, ShaderProgram const& shd) -> void {
            auto mvp_matrix = camera.get_view_projection_matrix() * en->module;
            auto mv_matrix = camera.get_view_matrix() * en->module;
            auto normal_matrix = glm::mat3(glm::transpose(glm::inverse(glm::mat3(en->module))));
            
            shd.bind();

            shd.set_mat4("module_view_matrix", mv_matrix);
            shd.set_mat4("mvp_matrix", mvp_matrix);
            shd.set_mat3("normal_matrix", normal_matrix);
            
            en->model.draw(shd);
            };

        auto shd_light_uniform = [&](ShaderProgram const& SHD) -> void {
            SHD.bind();

            SHD.set_float("material.shininess", 32.f);

            SHD.set_uint("PLA.size", point_lights.size());

            for (int i = 0; i < point_lights.size(); ++i) {
                auto name = std::format("PLA.pnts[{}]", i);
                const auto& cur = point_lights[i];

                auto position_eye = camera.get_view_matrix() * glm::vec4(cur.position, 1.f);
                SHD.set_vec3((name + ".position_eye").c_str(), glm::vec3(position_eye));
                SHD.set_vec3((name + ".ambient").c_str(), cur.ambient);
                SHD.set_vec3((name + ".diffuse").c_str(), cur.diffuse);
                SHD.set_vec3((name + ".specular").c_str(), cur.specular);
                SHD.set_float((name + ".shininess").c_str(), cur.shininess);
                SHD.set_float((name + ".linear").c_str(), cur.linear);
                SHD.set_float((name + ".quadro").c_str(), cur.quadro);
                SHD.set_float((name + ".intensity").c_str(), cur.intensity);

            }


            };
       

		while (!m_bCloseWindow) {

            Renderer_OpenGL::set_clear_color(m_background_color);

            shd_light_uniform(NSP);
            shd_light_uniform(CSP);

            Renderer_OpenGL::clear();

            soldier.module = glm::mat4(1.f);
            draw(&soldier, NSP);

            auto size = 21.0;
            auto resize1 = 0.1;
            cube.module = glm::translate(glm::scale(glm::mat4(1.f), { resize1, resize1, resize1}), { 0, 0, 0});
            draw(&cube, NSP);

            auto scf = 0.11;
            auto tsf = (scf * size - resize1 * size) / 2.0;
            cube.module = glm::scale(glm::mat4(1.f), { scf, scf, scf });
            
            CSP.bind();
            CSP.set_int("flag", 1);
            draw(&cube, CSP);
            
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

    void Application::set_title(const char* title) const {
        m_pWindow->set_title(title);
    }


} 