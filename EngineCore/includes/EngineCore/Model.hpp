#pragma once 

#include <string>
#include <vector>
#include <memory>

#include "EngineCore/Rendering/OpenGL/Mesh.hpp"
#include "EngineCore/Rendering/OpenGL/Texture2D.hpp"
#include "EngineCore/Rendering/OpenGL/ShaderProgram.hpp"
#include "EngineCore/Logs.hpp"

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

namespace EngineCore {

	class Model {
	private:
		std::vector<Mesh> meshes;
		std::string directory;

		void load_model(std::string path);
		void process_node(aiNode* node, const aiScene* scene);
		Mesh process_mesh(aiMesh* mesh, const aiScene* scene);

		std::vector<std::shared_ptr<Texture2D>> load_material_textures(aiMaterial* mat, aiTextureType type, Texture2D::type typeName);

	public:
		Model(const char* path) {
			load_model(path);
			LOG_INFO("MODEL LOADED FROM '{}'", path);
		}

		void draw(ShaderProgram const& shader);

	};

}