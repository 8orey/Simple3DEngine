#pragma once 
#include <glm/glm.hpp>

namespace EngineCore {

	using uint32_t = unsigned int;

	class ShaderProgram {
	public:
		ShaderProgram(const char* vertex_shader_src, const char* fragment_shader_src);
		ShaderProgram(ShaderProgram&&);
		ShaderProgram& operator=(ShaderProgram&&);
		~ShaderProgram();

		ShaderProgram() = delete;
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;

		void bind() const;
		static void unbind();
		bool is_compiled() const { return m_isCompiled; };
		void set_mat4(const char* name, const glm::mat4& mat) const;
		void set_mat3(const char* name, const glm::mat3& mat) const;
		void set_int(const char* name, const int num) const;
		void set_float(const char* name, const float num) const;
		void set_vec3(const char* name, const float x, const float y, const float z) const;
		void set_vec3(const char* name, const glm::vec3& vec) const;

	private:
		bool m_isCompiled = false;
		uint32_t m_id = 0;
	};


}