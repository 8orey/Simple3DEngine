#include <EngineCore/Application.hpp>
#include <EngineCore/Logs.hpp>
#include <EngineCore/Input.hpp>
#include <EngineCore/Camera.hpp>
#include <EngineCore/Event.hpp>

#include "IMGUI/imgui.h"

float MOVE_SPEED = 0.025f;
float ROTATE_SPEED = 0.5f;

class Editor : public EngineCore::Application {

    double m_x_mouse_pos_l = 0.0f;
    double m_y_mouse_pos_l = 0.0f;
    bool m_perspective_camera = true;

    virtual void on_update() override {
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
                camera.move_right((m_x_mouse_pos_l - cursor_pos.x) / 100.0);
                camera.move_world_up((m_y_mouse_pos_l - cursor_pos.y) / 100.0);
                m_x_mouse_pos_l = cursor_pos.x;
                m_y_mouse_pos_l = cursor_pos.y;
            }
            else {   
                rot_delta.z += (m_x_mouse_pos_l - cursor_pos.x) / 5.0;
                rot_delta.y -= (m_y_mouse_pos_l - cursor_pos.y) / 5.0;
                m_x_mouse_pos_l = cursor_pos.x; 
                m_y_mouse_pos_l = cursor_pos.y;
            }
        }

        if (EngineCore::Input::is_key_pressed(EngineCore::KeyCode::KEY_LEFT_SHIFT)) {
            move_delta *= 2;
        }

        camera.add_movement_and_rotation(move_delta, rot_delta);
        camera.set_ptojection_mode(
            (m_perspective_camera ? EngineCore::Camera::ProjectionMode::Perspective : EngineCore::Camera::ProjectionMode::Orthographic)
        );
    }

    void on_mouse_key_activity(const EngineCore::MouseKeyCode key_code, const double x, const double y, const bool pressed) override {
        m_x_mouse_pos_l = x;
        m_y_mouse_pos_l = y;
    }

    virtual void on_main_UI_update() override {
        ImGui::Checkbox("Perspective Camera", &m_perspective_camera);
    }

    virtual void on_UI_update() override {};
};

int main() {
    Editor App;
    auto exitCode = App.start(512, 512, "3D Engine");
    return exitCode;
}
