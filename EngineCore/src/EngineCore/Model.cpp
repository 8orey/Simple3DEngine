#include "EngineCore/Model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "EngineCore/Modules/FileRead.hpp"

#include "EngineCore/Logs.hpp"

#include "EngineCore/Rendering/OpenGL/Mesh.hpp"
#include "EngineCore/Rendering/OpenGL/Texture2D.hpp"
#include "EngineCore/Rendering/OpenGL/ShaderProgram.hpp"

#include <string>
#include <vector>
#include <memory>

namespace EngineCore {

	void Model::load_model(std::string path) {
		auto scene = import_scene(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (scene == nullptr) {
			LOG_ERROR("LOAD_MODEL_ERROR: {}", path);
			return;
		}

		directory = path.substr(0, path.find_last_of('/') + 1);
		process_node(scene->mRootNode, scene);
	}

	void Model::draw(ShaderProgram const& shader) {
		shader.bind();
		size_t cnt = 0;
		for (auto const& mesh : meshes) {
			mesh.draw(shader);
			++cnt;
		}
	}

	void Model::process_node(aiNode* node, const aiScene* scene) {
		for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(process_mesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; ++i) {
			process_node(node->mChildren[i], scene);
		}
	}

	Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<std::shared_ptr<Texture2D>> textures;

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

			std::vector<std::shared_ptr<Texture2D>> diffusemaps(load_material_textures(material, aiTextureType_DIFFUSE, Texture2D::type::diffuse));
			textures.insert(textures.end(), diffusemaps.begin(), diffusemaps.end());

			std::vector<std::shared_ptr<Texture2D>> specularmaps(load_material_textures(material, aiTextureType_SPECULAR, Texture2D::type::specular));
			textures.insert(textures.end(), specularmaps.begin(), specularmaps.end());
		}

		return Mesh(vertices, std::move(textures), indices, StandartPNT_layout, VertexBuffer::EUsage::Dynamic);
	}

	std::vector<std::shared_ptr<Texture2D>> Model::load_material_textures(aiMaterial* mat, aiTextureType assimp_type, Texture2D::type type) {
		std::vector<std::shared_ptr<Texture2D>> in_textures;
		for (unsigned int i = 0; i < (mat->GetTextureCount(assimp_type)); ++i) {
			aiString str;
			mat->GetTexture(assimp_type, i, &str);
			in_textures.push_back(std::make_shared<Texture2D>(read_image((directory + str.C_Str()).c_str()), type));
		}
		return in_textures;
	}
	



}