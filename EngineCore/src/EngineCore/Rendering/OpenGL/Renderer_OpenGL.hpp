#pragma once 

struct GLFWwindow;

namespace EngineCore {
	using uint32_t = unsigned int;
	class VertexArray;

	class Renderer_OpenGL {
	public:
		static bool init(GLFWwindow* pWindow, const bool debug);

		static void draw(const VertexArray& vertex_arr);
		static void set_clear_color(const float color[4]);
		static void clear();
		static void set_viewport(const uint32_t width, const uint32_t height, const uint32_t left_offset = 0, const uint32_t bottom_offset = 0);
		static void enable_depth_testing();
		static void disable_depth_testing();

		static const char* get_vendor_str();
		static const char* get_renderer_str();
		static const char* get_version_str();
	};
}


