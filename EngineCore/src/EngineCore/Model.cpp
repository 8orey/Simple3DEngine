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



	void Model::process_node(aiNode* node, const aiScene* scene) {
		for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.emplace_back(mesh, scene, directory.c_str());
		}

		for (uint32_t i = 0; i < node->mNumChildren; ++i) {
			process_node(node->mChildren[i], scene);
		}
	}

	void Model::draw(ShaderProgram const& shader) {
		shader.bind();
		for (auto const& mesh : meshes) {
			mesh.draw(shader);
		}
	}

	void Model::raw_draw(ShaderProgram const& shader) {
		shader.bind();
		for (auto const& mesh : meshes) {
			mesh.raw_draw(shader);
		}
	}


	 
	



}