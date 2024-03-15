#include "EngineCore/Rendering/OpenGL/Mesh.hpp"
#include "EngineCore/Logs.hpp"

#include <glad/glad.h>

#include <vector>
#include <string>
#include <format>

#include "EngineCore/Rendering/OpenGL/VertexArray.hpp"
#include "EngineCore/Rendering/OpenGL/VertexBuffer.hpp"
#include "EngineCore/Rendering/OpenGL/IndexBuffer.hpp"
#include "EngineCore/Rendering/OpenGL/Texture2D.hpp"
#include "EngineCore/Rendering/OpenGL/ShaderProgram.hpp"
#include "EngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"

namespace EngineCore {

	Mesh::Mesh(
		std::vector<Vertex> const& vertices,
		std::vector<std::shared_ptr<Texture2D>> const& textures,
		std::vector<GLuint> indices,
		BufferLayout layout,
		VertexBuffer::EUsage usage
	):
		vertices(vertices),
		textures(textures),
		VBO(vertices, layout, usage),
		VAO(),
		IBO(indices, usage)
	{
		VAO.add_vertex_buffer(VBO);
		VAO.set_index_buffer(IBO);
	}


	std::pair<std::string, size_t> texture_type_to_string(Texture2D::type val, size_t index_array[3]) {
		switch (val) {
		case Texture2D::type::ambient: {
			return { "ambient", index_array[0]++};
		}
		case Texture2D::type::diffuse: {
			return { "diffuse", index_array[1]++};
		}
		case Texture2D::type::specular: {
			return { "specular", index_array[2]++};
		}
		}
		LOG_ERROR("texture_type_to_string[MESH.cpp]: Unknown texture type");
		return { "none", -1 };
	}

	void Mesh::draw(ShaderProgram& shader) {
		shader.bind();

		size_t index_array[3] = { 0, 0, 0 };
		size_t cur_index = 0;

		for (auto const& texture : textures) {
			auto [name, index] = texture_type_to_string(texture->get_type(), index_array);
			shader.set_int(std::format("material.{}{}", name, index).c_str(), cur_index);
			texture->bind(cur_index++);
		}

		Renderer_OpenGL::draw(VAO);
		shader.unbind();
	}


}