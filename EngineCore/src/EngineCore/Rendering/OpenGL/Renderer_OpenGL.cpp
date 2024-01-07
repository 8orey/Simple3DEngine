#include "Renderer_OpenGL.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VertexArray.hpp"
#include "EngineCore/Logs.hpp"

namespace EngineCore {


	bool Renderer_OpenGL::init(GLFWwindow* pWindow) {
		glfwMakeContextCurrent(pWindow);

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
			LOG_CRITICAL("Failed to initialize GLAD");
			return false;
		}

		LOG_INFO("OpenGL context initialized:");
		LOG_INFO("  Vendor: {}", get_vendor_str());
		LOG_INFO("  Renderer: {}", get_renderer_str());
		LOG_INFO("  Version: {}", get_version_str());

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