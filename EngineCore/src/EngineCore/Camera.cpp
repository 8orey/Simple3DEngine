
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "EngineCore/Camera.hpp"

namespace EngineCore {


	Camera::Camera(const glm::vec3& position,
		const glm::vec3& rotation,
		const ProjectionMode projection_mode)
		: m_position(position)
		, m_rotation(rotation)
		, m_projection_mode(projection_mode) 
	{
		update_view_matrix();
		update_projection_matrix();
	}

	void Camera::update_view_matrix() {

		const float roll_rad = glm::radians(m_rotation.x);
		const float pitch_rad = glm::radians(m_rotation.y);
		const float yaw_rad = glm::radians(m_rotation.z);

		const glm::mat3 r_mat_x(
			1, 0, 0, 
			0, cos(roll_rad), sin(roll_rad),
			0, -sin(roll_rad), cos(roll_rad)
		);

		const glm::mat3 r_mat_y(
			cos(pitch_rad), 0, -sin(pitch_rad),
			0, 1, 0,
			sin(pitch_rad), 0, cos(pitch_rad)
		);

		const glm::mat3 r_mat_z(
			cos(yaw_rad), sin(yaw_rad), 0,
			-sin(yaw_rad), cos(yaw_rad), 0,
			0, 0, 1
		);

		const glm::mat3 eulear_rotate_matrix = r_mat_z * r_mat_y * r_mat_x;

		m_direction = glm::normalize(eulear_rotate_matrix * s_world_forward);
		m_right = glm::normalize(eulear_rotate_matrix * s_world_right);
		m_up = glm::cross(m_right, m_direction);
		
		m_view_matrix = glm::lookAt(m_position, m_position + m_direction, m_up);
	};
	void Camera::update_projection_matrix() {
		if (m_projection_mode == ProjectionMode::Perspective) {
			float r = 0.1f, t = 0.1f, f = 100.f, n = 0.1f;
			m_projection_matrix = glm::mat4(
				n / r, 0, 0, 0,
				0, n / t, 0, 0,
				0, 0, (-f - n) / (f - n), -1,
				0, 0, -2 * f * n / (f - n), 0
			);
		}
		else {
			float r = 2.f, t = 2.f, f = 100.f, n = 0.1f;
			m_projection_matrix = glm::mat4(
				1 / r, 0, 0, 0,
				0, 1 / t, 0, 0,
				0, 0, -2 / (f - n), 0,
				0, 0, (-f - n) / (f - n), 1
			);
		}
	};


	void Camera::set_position(const glm::vec3& position) {
		if (position != m_position) {
			m_should_update_view_matrix = true;
			m_position = position;
		}
	}
	
	void Camera::set_rotation(const glm::vec3& rotation) {
		if (rotation != m_rotation) {
			m_should_update_view_matrix = true;
			m_rotation = rotation;
		}
	}

	void Camera::set_projection_mode(const ProjectionMode projection_mode) {
		if (m_projection_mode != projection_mode) {
			m_projection_mode = projection_mode;
			update_projection_matrix();
		}
	}


	void Camera::move_forward(const float delta) {	
		if (delta != 0.0f) {
			m_should_update_view_matrix = true;
			m_position += delta * m_direction;
		}
	};

	void Camera::move_right(const float delta) {
		if (delta != 0.0f) {
			m_should_update_view_matrix = true;
			m_position += delta * m_right;
		}
	};
	void Camera::move_world_up(const float delta) {
		if (delta != 0.0f) {
			m_should_update_view_matrix = true;
			m_position += delta * s_world_up;
		}
	};

	void Camera::add_movement_and_rotation(const glm::vec3& move_delta, const glm::vec3& rot_delta) {
		
		if ((move_delta.x != 0.0f) ||
			(move_delta.y != 0.0f) ||
			(move_delta.z != 0.0f) ||
			(rot_delta.x != 0.0f) ||
			(rot_delta.y != 0.0f) ||
			(rot_delta.z != 0.0f)) {
			m_should_update_view_matrix = true;
			m_position += m_direction * move_delta.x;
			m_position += m_right * move_delta.y;
			m_position += m_up * move_delta.z;
			m_rotation += rot_delta;
		}
	}

}