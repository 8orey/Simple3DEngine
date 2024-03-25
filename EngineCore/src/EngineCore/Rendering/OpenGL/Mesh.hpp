#pragma once 

#include <glad/glad.h>
#include <vector>
#include <memory>

#include "EngineCore/Rendering/OpenGL/VertexArray.hpp"
#include "EngineCore/Rendering/OpenGL/VertexBuffer.hpp"
#include "EngineCore/Rendering/OpenGL/IndexBuffer.hpp"
#include "EngineCore/Rendering/OpenGL/Texture2D.hpp"
#include "EngineCore/Rendering/OpenGL/ShaderProgram.hpp"
#include "EngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"

struct aiMesh;
struct aiScene;
struct aiMaterial;

enum aiTextureType;


namespace EngineCore {

	class Mesh {
	public:
		Mesh(
			std::vector<Vertex> const& vertices,
			std::vector<Texture2D>& textures,
			std::vector<GLuint> indices,
			BufferLayout layout,
			VertexBuffer::EUsage usage
		);

		Mesh(aiMesh* mesh, const aiScene* scene, const char* directory);

		Mesh& operator=(Mesh const&) = delete;
		Mesh(Mesh const&) = delete;
		Mesh& operator=(Mesh&&) = default;
		Mesh(Mesh&&) = default;

		void draw(ShaderProgram const& shader) const;

		void raw_draw(ShaderProgram const& shader) const;

	private:

		void load_material_textures(
			aiMaterial* mat, aiTextureType assimp_type, Texture2D::type type, std::string const& directory
		);

		std::vector<Vertex> vertices;
		std::vector<Texture2D> textures;

		std::unique_ptr<VertexBuffer> pVBO;
		std::unique_ptr<VertexArray> pVAO;
		std::unique_ptr<IndexBuffer> pIBO;
	};






}


