
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "EngineCore/Camera.hpp"
#include <glm/ext/matrix_clip_space.hpp>

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
			m_projection_matrix = glm::perspective(
				m_field_of_view, 
				(m_viewport_height != 0 ? m_viewport_width / m_viewport_height : 1.f/2.f),
				m_near_clip_plane,
				m_far_clip_plane);
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


	void  Camera::set_position(const glm::vec3& position) {
		if (position != m_position) {
			m_should_update_view_matrix = true;
			m_position = position;
		}
	}

	void  Camera::set_rotation(const glm::vec3& rotation) {
		if (rotation != m_rotation) {
			m_should_update_view_matrix = true;
			m_rotation = rotation;
		}
	}

	void  Camera::set_projection_mode(const ProjectionMode projection_mode) {
		if (m_projection_mode != projection_mode) {
			m_should_update_projecting_matrix = true;
			m_projection_mode = projection_mode;
		}
	}

	void  Camera::set_near_plane(const float near) {
		if (near != m_near_clip_plane) {
			m_should_update_projecting_matrix = true;
			m_near_clip_plane = near;
		}
	};

	void  Camera::set_viewport_size(const float width, const float height) {
		if (width != m_viewport_width or height != m_viewport_height) {
			m_should_update_projecting_matrix = true;
			m_viewport_width = width;
			m_viewport_height = height;
		}
	};

	void  Camera::set_field_of_view(const float fov) {
		if (m_field_of_view != fov) {
			m_should_update_projecting_matrix = true;
			m_field_of_view = fov;
		};
	};

	void  Camera::set_far_plane(const float far) {
		if (far != m_far_clip_plane) {
			m_should_update_projecting_matrix = true;
			m_far_clip_plane = far;
		}
	};

	glm::vec3  Camera::get_position(const glm::vec3& position) const {
		return m_position;
	};
	glm::vec3  Camera::get_rotation(const glm::vec3& rotation) const {
		return m_rotation;
	};
	Camera::ProjectionMode  Camera::get_projection_mode() const {
		return m_projection_mode;
	};

	float  Camera::get_far_plane() const {
		return m_far_clip_plane;
	}
	float  Camera::get_near_plane() const {
		return m_near_clip_plane;
	};
	float  Camera::get_viewport_height() const {
		return m_viewport_height;
	};
	float  Camera::get_viewport_width() const {
		return m_viewport_width;
	}
	float  Camera::get_field_of_view() const {
		return m_field_of_view;
	};

	glm::mat4 Camera::get_view_matrix() {
		if (m_should_update_view_matrix) {
			update_view_matrix();
			m_should_update_view_matrix = false;
		}
		return m_view_matrix;
	}
	glm::mat4 Camera::get_projection_matrix() {
		if (m_should_update_projecting_matrix) {
			update_projection_matrix();
			m_should_update_projecting_matrix = false;
		}
		return m_projection_matrix;
	}

	void Camera::move_forward(const float delta) {
		if (delta != 0.f) {
			m_should_update_view_matrix = true;
			m_position += delta * m_direction;
		}
	};
	void Camera::move_right(const float delta) {
		if (delta != 0.f) {
			m_should_update_view_matrix = true;
			m_position += delta * m_right;
		}
	};
	void Camera::move_world_up(const float delta) {
		if (delta != 0.f) {
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