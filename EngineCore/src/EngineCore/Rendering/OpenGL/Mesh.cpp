#include "EngineCore/Rendering/OpenGL/Mesh.hpp"
#include "EngineCore/Logs.hpp"

#include <glad/glad.h>

#include <vector>
#include <string>
#include <format>
#include <memory>

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
		pVBO(std::make_shared<VertexBuffer>(vertices, layout, usage)),
		pVAO(std::make_shared<VertexArray>()),
		pIBO(std::make_shared<IndexBuffer>(indices, usage))
	{
		pVAO->add_vertex_buffer(*pVBO);
		pVAO->set_index_buffer(*pIBO);
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

	void Mesh::draw(ShaderProgram const& shader) const {
		shader.bind();

		size_t index_array[3] = { 0, 0, 0 };
		int cur_index = 0;

		for (auto const& texture : textures) {

			if (texture == nullptr) {
				LOG_ERROR("[Mesh] EMPTY TEXTURE");
			}

			auto [name, index] = texture_type_to_string(texture->get_type(), index_array);
			shader.set_int(std::format("material.{}{}", name, index).c_str(), cur_index);
			texture->bind(cur_index++);
		}

		// LOG_INFO("MESH_TEXURES_DATA: ambient = {} | diffuse = {} | specular = {}", index_array[0], index_array[1], index_array[2]);

		Renderer_OpenGL::draw(*pVAO);
	}


}