#include <EngineCore/Application.hpp>
#include <EngineCore/Logs.hpp>
#include <EngineCore/Input.hpp>
#include <EngineCore/Camera.hpp>
#include <EngineCore/Event.hpp>

#include "IMGUI/imgui.h"
#include <imgui_internal.h>
#include <glm/trigonometric.hpp>

float MOVE_SPEED = 0.025f;
float ROTATE_SPEED = 0.5f;

class Editor : public EngineCore::Application {

    float m_x_mouse_pos_l = 0.0f;
    float m_y_mouse_pos_l = 0.0f;
    bool m_perspective_camera = true;

    float camera_far{};
    float camera_near{};
    float camera_fov{};
    float camera_viewport_w{};
    float camera_viewport_h{};

    void init() override {
        camera.set_far_plane(100.f);
        camera.set_near_plane(0.1f);
        camera.set_field_of_view(glm::radians(80.f));
    
        camera_far = camera.get_far_plane();
        camera_near = camera.get_near_plane();
        camera_fov = camera.get_field_of_view();
        camera_viewport_w = camera.get_viewport_width();
        camera_viewport_h = camera.get_viewport_height();
    }

    void camera_pos_update() {
        glm::vec3 move_delta{ 0, 0, 0 };
        glm::vec3 rot_delta{ 0, 0, 0 };

        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_W)) {
            move_delta.x += MOVE_SPEED;
        }
        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_S)) {
            move_delta.x -= MOVE_SPEED;
        }
        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_A)) {
            move_delta.y -= MOVE_SPEED;
        }
        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_D)) {
            move_delta.y += MOVE_SPEED;
        }
        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_Q)) {
            move_delta.z += MOVE_SPEED;
        }
        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_E)) {
            move_delta.z -= MOVE_SPEED;
        }

        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_UP)) {
            rot_delta.y -= ROTATE_SPEED;
        }
        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_DOWN)) {
            rot_delta.y += ROTATE_SPEED;
        }
        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_RIGHT)) {
            rot_delta.z -= ROTATE_SPEED;
        }
        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_LEFT)) {
            rot_delta.z += ROTATE_SPEED;
        }
        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_P)) {
            rot_delta.x += ROTATE_SPEED;
        }
        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_O)) {
            rot_delta.x -= ROTATE_SPEED;
        }

        if (EngineCore::Input::is_mouse_key_pressed(EngineCore::MouseKeyCode::MOUSE_BUTTON_RIGHT)) {
            glm::vec2 cursor_pos = get_current_mouse_position();
            if (EngineCore::Input::is_mouse_key_pressed(EngineCore::MouseKeyCode::MOUSE_BUTTON_LEFT)) {
                camera.move_right((m_x_mouse_pos_l - cursor_pos.x) / 100.0f);
                camera.move_world_up((m_y_mouse_pos_l - cursor_pos.y) / 100.0f);
                m_x_mouse_pos_l = cursor_pos.x;
                m_y_mouse_pos_l = cursor_pos.y;
            }
            else {
                rot_delta.z += (m_x_mouse_pos_l - cursor_pos.x) / 5.0f;
                rot_delta.y -= (m_y_mouse_pos_l - cursor_pos.y) / 5.0f;
                m_x_mouse_pos_l = cursor_pos.x;
                m_y_mouse_pos_l = cursor_pos.y;
            }
        }

        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_LEFT_SHIFT)) {
            move_delta *= 2;
        }

        camera.add_movement_and_rotation(move_delta, rot_delta);
        camera.set_projection_mode(
            (m_perspective_camera ? EngineCore::Camera::ProjectionMode::Perspective : EngineCore::Camera::ProjectionMode::Orthographic)
        );
    }

    void on_update() override {
        camera_pos_update();
    }

    void on_mouse_key_activity(const EngineCore::MouseKeyCode key_code, const float x, const float y, const bool pressed) override {
        m_x_mouse_pos_l = x;
        m_y_mouse_pos_l = y;
    }

    void on_UI_update() override {
        setup_dockspace_menu();

        camera_far = camera.get_far_plane();
        camera_fov = camera.get_field_of_view();

        ImGui::Begin("Camera options");

        if (ImGui::SliderFloat("Far plain", &camera_far, 10, 150)) {
            camera.set_far_plane(camera_far);
        }
        if (ImGui::SliderAngle("FOV", &camera_fov, 50, 110)) {
            camera.set_field_of_view(camera_fov);
        }
        if (ImGui::Checkbox("Perspective Camera", &m_perspective_camera)) {
            camera.set_projection_mode((m_perspective_camera ? EngineCore::Camera::ProjectionMode::Perspective : EngineCore::Camera::ProjectionMode::Orthographic));
        }
        ImGui::SliderFloat3("Light pos", light_pos, -10, 10);
        ImGui::ColorEdit3("Light col", light_col);
        ImGui::SliderFloat("Ambient factor", &ambient_factor, 0.1f, 5.f);
        ImGui::SliderFloat("Diffuse factor", &diffuse_factor, 0.1f, 5.f);
        ImGui::SliderFloat("Specular factor", &specular_factor, 0.1f, 5.f);
        ImGui::SliderFloat("Shinnines", &shine_factor, 5.f, 100.f);
        ImGui::End();

    };

    void setup_dockspace_menu() {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoWindowMenuButton;
        static ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene...", NULL))
                {

                }
                if (ImGui::MenuItem("Open Scene...", NULL))
                {

                }
                if (ImGui::MenuItem("Save Scene...", NULL))
                {

                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", NULL))
                {
                    close();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }

};



int main() {
    Editor App;
    auto exitCode = App.start(512, 512, "3D_Engine");
    return exitCode;
}
