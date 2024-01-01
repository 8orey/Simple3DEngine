
#include <glm/trigonometric.hpp>
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

		float r_rad_x = glm::radians(-m_rotation.x);
		glm::mat4 r_mat_x(
			1,            0,            0, 0,
			0, cos(r_rad_x), sin(r_rad_x), 0,
			0,-sin(r_rad_x), cos(r_rad_x), 0,
			0,            0,            0, 1
		);

		float r_rad_y = glm::radians(-m_rotation.y);
		glm::mat4 r_mat_y(
			cos(r_rad_y), 0, -sin(r_rad_y), 0,
			           0, 1,             0, 0,
			sin(r_rad_y), 0,  cos(r_rad_y), 0,
			           0, 0,             0, 1
		);

		float r_rad_z = glm::radians(-m_rotation.z);
		glm::mat4 r_mat_z(
			cos(r_rad_z), sin(r_rad_z), 0, 0,
		   -sin(r_rad_z), cos(r_rad_z), 0, 0,
			           0,            0, 1, 0,
			           0,            0, 0, 1
		);

		glm::mat4 translate_matrix(
			             1,              0,              0, 0,
			             0,              1,              0, 0,
			             0,              0,              1, 0,
			-m_position[0], -m_position[1], -m_position[2], 1
		);

		m_view_matrix = r_mat_y * r_mat_x * translate_matrix;
	};
	void Camera::update_projection_matrix() {
		if (m_projection_mode == ProjectionMode::Perspective) {
			float r = 0.1, t = 0.1, f = 10, n = 0.1;
			m_projection_matrix = glm::mat4(
				n / r,     0,                    0, 0, 
				    0, n / t,                    0, 0,
				    0,     0,  (- f - n) / (f - n), -1,
				    0,     0, -2 * f * n / (f - n), 0
				);
		}
		else {


		}



	};


	void Camera::set_position(const glm::vec3& position) {
		m_position = position;
		update_view_matrix();
	}
	
	void Camera::set_rotation(const glm::vec3& rotation) {
		m_rotation = rotation;
		update_view_matrix();
	}

	void Camera::set_position_rotation(const glm::vec3& position, const glm::vec3& rotation) {
		m_position = position;
		m_rotation = rotation;
		update_view_matrix();
	}

	void Camera::set_ptojection_mode(const ProjectionMode projection_mode) {
		m_projection_mode = projection_mode;
		update_projection_matrix();
	}

}