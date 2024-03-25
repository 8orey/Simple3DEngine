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
		std::vector<Texture2D>& textures,
		std::vector<GLuint> indices,
		BufferLayout layout,
		VertexBuffer::EUsage usage
	):
		vertices(vertices),
		textures(std::move(textures)),
		pVBO(std::make_unique<VertexBuffer>(vertices, layout, usage)),
		pVAO(std::make_unique<VertexArray>()),
		pIBO(std::make_unique<IndexBuffer>(indices, usage))
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
			auto [name, index] = texture_type_to_string(texture.get_type(), index_array);
			shader.set_int(std::format("material.{}{}", name, index).c_str(), cur_index);
			texture.bind(cur_index++);
		}

		// LOG_INFO("MESH_TEXURES_DATA: ambient = {} | diffuse = {} | specular = {}", index_array[0], index_array[1], index_array[2]);

		Renderer_OpenGL::draw(*pVAO);
	}
	
	void Mesh::raw_draw(ShaderProgram const& shader) const {
		Renderer_OpenGL::draw(*pVAO);
	}

	Mesh::Mesh(aiMesh* mesh, const aiScene* scene, const char* directory) {
		std::vector<GLuint> indices;

		for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
			Vertex vert;

			vert.position.x = mesh->mVertices[i].x;
			vert.position.y = mesh->mVertices[i].y;
			vert.position.z = mesh->mVertices[i].z;

			if (mesh->HasNormals()) {
				vert.normal.x = mesh->mNormals[i].x;
				vert.normal.y = mesh->mNormals[i].y;
				vert.normal.z = mesh->mNormals[i].z;
			}
			else {
				vert.normal = glm::vec3(0.0f);
			}

			if (mesh->mTextureCoords[0]) {
				vert.texture_position.x = mesh->mTextureCoords[0][i].x;
				vert.texture_position.y = mesh->mTextureCoords[0][i].y;
			}
			else {
				vert.texture_position = glm::vec2(0.0f);
			}

			vertices.push_back(vert);
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}

		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			load_material_textures(material, aiTextureType_DIFFUSE, Texture2D::type::diffuse, directory);
			load_material_textures(material, aiTextureType_SPECULAR, Texture2D::type::specular, directory);
		}

		BufferLayout layout = StandartPNT_layout;
		auto usage = VertexBuffer::EUsage::Dynamic;

		pVBO = std::make_unique<VertexBuffer>(vertices, layout, usage);
		pVAO = std::make_unique<VertexArray>();
		pIBO = std::make_unique<IndexBuffer>(indices, usage);
		pVAO->add_vertex_buffer(*pVBO);
		pVAO->set_index_buffer(*pIBO);
	}

	void Mesh::load_material_textures(
		aiMaterial* mat, aiTextureType assimp_type, Texture2D::type type, std::string const &directory
	) {
		for (unsigned int i = 0; i < (mat->GetTextureCount(assimp_type)); ++i) {
			aiString str;
			mat->GetTexture(assimp_type, i, &str);
			textures.emplace_back(Texture2D(read_image((directory + str.C_Str()).c_str()), type));
		}
	}

	


}