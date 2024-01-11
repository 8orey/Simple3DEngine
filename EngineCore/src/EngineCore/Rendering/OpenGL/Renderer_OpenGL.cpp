#include "Renderer_OpenGL.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VertexArray.hpp"
#include "EngineCore/Logs.hpp"


namespace EngineCore {
	

	const char* get_source_description(GLenum source) {
		switch (source) {
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW_SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD_PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
		LOG_ERROR("[UNKNOWN OPENGL ERROR SOURCE]: {}", source);
		return "-1";
	}

	const char* get_type_description(GLenum type) {
		switch (type) {
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_PUSH_GROUP: return "PUSH_GROUP";
		case GL_DEBUG_TYPE_POP_GROUP: return "POP_GROUP";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
		LOG_ERROR("[UNKNOWN OPENGL ERROR TYPE]: {}", type);
		return "-1";
	}

	void OpenGL_Debug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void*) {
		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			LOG_ERROR("OpenGL Error: [{0}:{1}]({2}): {3}",
				get_source_description(source),
				get_type_description(type),
				id,
				message);
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			LOG_WARN("OpenGL Warning: [{0}:{1}]({2}): {3}",
				get_source_description(source),
				get_type_description(type),
				id,
				message);
			break;
		case GL_DEBUG_SEVERITY_LOW:
			LOG_INFO("OpenGL Info: [{0}:{1}]({2}): {3}",
				get_source_description(source),
				get_type_description(type),
				id,
				message);
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			LOG_INFO("OpenGL Notificaton: [{0}:{1}]({2}): {3}",
				get_source_description(source),
				get_type_description(type),
				id,
				message);
			break;
		default:
			LOG_ERROR("OpenGL Error: [{0}:{1}] ({2}) : {3}",
				get_source_description(source),
				get_type_description(type),
				id,
				message);
		}
	}
	

	bool Renderer_OpenGL::init(GLFWwindow* pWindow, const bool debug) {
		glfwMakeContextCurrent(pWindow);

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
			LOG_CRITICAL("Failed to initialize GLAD");
			return false;
		}

		LOG_INFO("OpenGL context initialized:");
		LOG_INFO("  Vendor: {}", get_vendor_str());
		LOG_INFO("  Renderer: {}", get_renderer_str());
		LOG_INFO("  Version: {}", get_version_str());

		if (debug) {
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
			glDebugMessageCallback(OpenGL_Debug, nullptr);
		}
		else {
			glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}
		return true;
	}

	void Renderer_OpenGL::draw(const VertexArray& vertex_arr) {
		vertex_arr.bind();
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertex_arr.get_indicies_count()), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer_OpenGL::set_clear_color(const float color[4]) {
		glClearColor(color[0], color[1], color[2], color[3]);
	}

	void Renderer_OpenGL::clear() {
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void Renderer_OpenGL::set_viewport(const uint32_t width, const uint32_t height, const uint32_t left_offset, const uint32_t bottom_offset) {
		glViewport(left_offset, bottom_offset, width, height);
	}

	const char* Renderer_OpenGL::get_vendor_str() {
		return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	}

	const char* Renderer_OpenGL::get_renderer_str() {
		return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	}

	const char* Renderer_OpenGL::get_version_str() {
		return reinterpret_cast<const char*>(glGetString(GL_VERSION));
	}

	void Renderer_OpenGL::enable_depth_testing() {
		glEnable(GL_DEPTH_TEST);
	}

	void Renderer_OpenGL::disable_depth_testing() {
		glDisable(GL_DEPTH_TEST);
	};

}