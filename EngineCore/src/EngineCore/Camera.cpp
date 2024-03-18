
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

		const glm::mat3 rotate_by_x = glm::rotate(glm::mat4(1.f), roll_rad, glm::vec3(1.f, 0.f, 0.f));
		const glm::mat3 rotate_by_y = glm::rotate(glm::mat4(1.f), pitch_rad, glm::vec3(0.f, 1.f, 0.f));
		const glm::mat3 rotate_by_z = glm::rotate(glm::mat4(1.f), yaw_rad, glm::vec3(0.f, 0.f, 1.f));

		const glm::mat3 eulear_rotate_matrix = rotate_by_z * rotate_by_y * rotate_by_x;

		m_direction = glm::normalize(eulear_rotate_matrix * s_world_forward);
		m_right = glm::normalize(eulear_rotate_matrix * s_world_right);
		m_up = glm::cross(m_right, m_direction);
		
		m_view_matrix = glm::lookAt(m_position, m_position + m_direction, m_up);
	};
	void Camera::update_projection_matrix() {
		if (m_projection_mode == ProjectionMode::Perspective) {
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
		m_projection_mode = projection_mode;
		update_projection_matrix();
	}

	void  Camera::set_near_plane(const float near) {
		m_near_clip_plane = near;
		update_projection_matrix();
	};

	void  Camera::set_viewport_size(const float width, const float height) {
		m_viewport_width = width;
		m_viewport_height = height;
		update_projection_matrix();
	};

	void  Camera::set_field_of_view(const float fov) {
		m_field_of_view = fov;
		update_projection_matrix();
	};

	void  Camera::set_far_plane(const float far) {
		m_far_clip_plane = far;
		update_projection_matrix();
	};

	glm::vec3  Camera::get_position() const {
		return m_position;
	};
	glm::vec3  Camera::get_rotation() const {
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
	const glm::mat4& Camera::get_view_matrix() {
		if (m_should_update_view_matrix) {
			update_view_matrix();
			m_should_update_view_matrix = false;
		}
		return m_view_matrix;
	}
	const glm::mat4& Camera::get_projection_matrix() const {
		return m_projection_matrix;
	}

	glm::mat4 Camera::get_view_projection_matrix() const {
		return m_projection_matrix * m_view_matrix;
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
		if ( move_delta!=glm::vec3(0.f) || rot_delta != glm::vec3(0.f)) {
			m_should_update_view_matrix = true;
			m_position += m_direction * move_delta.x;
			m_position += m_right * move_delta.y;
			m_position += m_up * move_delta.z;
			m_rotation += rot_delta;
		}
	}

}