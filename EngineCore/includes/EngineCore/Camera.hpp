#pragma once 

#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_float4x4.hpp>


namespace EngineCore {

	class Camera {

	public:
		enum class ProjectionMode {
			Perspective,
			Orthographic,
		};

		Camera(const glm::vec3& position = { 0, 0, 0 },
			const glm::vec3& rotation = { 0, 0, 0 },
			const ProjectionMode projection_mode = ProjectionMode::Perspective);

		void set_position(const glm::vec3& position);
		void set_rotation(const glm::vec3& rotation);
		void set_projection_mode(const ProjectionMode projection_mode);

		void set_near_plane(const float near);
		void set_viewport_size(const float width, const float height);
		void set_field_of_view(const float fov);
		void set_far_plane(const float far);

		glm::vec3 get_position() const;
		glm::vec3 get_rotation() const;
		ProjectionMode get_projection_mode() const;

		float get_far_plane() const;
		float get_near_plane() const;
		float get_viewport_height() const;
		float get_viewport_width() const;
		float get_field_of_view() const;

		const glm::mat4& get_view_matrix();
		const glm::mat4& get_projection_matrix() const;

		void move_forward(const float delta);
		void move_right(const float delta);
		void move_world_up(const float delta);

		void add_movement_and_rotation(const glm::vec3& move_delta, const glm::vec3& rot_delta);

	private:

		void update_view_matrix();
		void update_projection_matrix();

		glm::vec3 m_position;
		glm::vec3 m_rotation;
		ProjectionMode m_projection_mode;

		static constexpr glm::vec3 s_world_up{ 0.f, 0.f, 1.f };
		static constexpr glm::vec3 s_world_right{ 0.f, -1.f, 0.f };
		static constexpr glm::vec3 s_world_forward{ 1.f, 0.f, 0.f };

		glm::vec3 m_direction = s_world_forward;
		glm::vec3 m_right = s_world_right;
		glm::vec3 m_up = s_world_up;

		float m_far_clip_plane{ 100.f };
		float m_near_clip_plane{ 0.1f };
		float m_viewport_width{ 800.f };
		float m_viewport_height{ 600.f };
		float m_field_of_view{ glm::radians(80.f) };

		glm::mat4 m_view_matrix;
		glm::mat4 m_projection_matrix;

		bool m_should_update_view_matrix = false;
	};

}



