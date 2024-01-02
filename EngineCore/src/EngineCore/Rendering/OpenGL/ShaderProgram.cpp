
#include "ShaderProgram.hpp"
#include "EngineCore/Logs.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace EngineCore {

	bool create_shader(const char* source, const GLenum shader_type, GLuint& shader_id) {
		shader_id = glCreateShader(shader_type);
		glShaderSource(shader_id, 1, &source, nullptr);
		glCompileShader(shader_id);

		GLint res;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);
		if (res == GL_FALSE) {
			char log[1024];
			glGetShaderInfoLog(shader_id, 1024, nullptr, log);

			LOG_CRITICAL("Shader compilation error: \n{}", log);
			return false;
		}
		return true;
	}

	ShaderProgram::ShaderProgram(const char* vertex_shader_src, const char* fragment_shader_src) {
		GLuint vertex_shader_id = 0;
		if (!create_shader(vertex_shader_src, GL_VERTEX_SHADER, vertex_shader_id)) {
			LOG_CRITICAL("Vertex shader compile error!");
			glDeleteShader(vertex_shader_id);
			return;
		}

		GLuint fragment_shader_id = 0;
		if (!create_shader(fragment_shader_src, GL_FRAGMENT_SHADER, fragment_shader_id)) {
			LOG_CRITICAL("Fragment shader compile error!");
			glDeleteShader(vertex_shader_id);
			glDeleteShader(fragment_shader_id);
			return;
		}

		m_id = glCreateProgram();
		glAttachShader(m_id, vertex_shader_id);
		glAttachShader(m_id, fragment_shader_id);
		glLinkProgram(m_id);

		GLint res;
		glGetProgramiv(m_id, GL_LINK_STATUS, &res);
		if (res == GL_FALSE) {
			GLchar log[1024];
			glGetProgramInfoLog(m_id, 1024, nullptr, log);
			LOG_CRITICAL("SHADER PROGRAM: Link-time error:\n{}", log);
			glDeleteProgram(m_id);
			m_id = 0;
			glDeleteShader(vertex_shader_id);
			glDeleteShader(fragment_shader_id);
			return;
		}
		else {
			m_isCompiled = true;
		}

		glDetachShader(m_id, vertex_shader_id);
		glDetachShader(m_id, fragment_shader_id);
		glDeleteShader(vertex_shader_id);
		glDeleteShader(fragment_shader_id);
	}

	ShaderProgram::~ShaderProgram() {
		glDeleteProgram(m_id);
	}

	void ShaderProgram::bind() const {
		glUseProgram(m_id);
	}

	void ShaderProgram::unbind() {
		glUseProgram(0);
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderProgram) {
		glDeleteProgram(m_id);
		m_id = shaderProgram.m_id;
		m_isCompiled = shaderProgram.m_isCompiled;

		shaderProgram.m_id = 0;
		shaderProgram.m_isCompiled = false;
		return *this;
	}

	ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram) {
		m_id = shaderProgram.m_id;
		m_isCompiled = shaderProgram.m_isCompiled;

		shaderProgram.m_id = 0;
		shaderProgram.m_isCompiled = false;
	}

	void ShaderProgram::set_mat4(const char* name, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(mat));
	}



}