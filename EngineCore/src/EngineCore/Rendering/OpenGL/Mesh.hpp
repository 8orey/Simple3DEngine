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


namespace EngineCore {

	class Mesh {
	public:
		std::vector<Vertex> vertices;
		std::vector<std::shared_ptr<Texture2D>> textures;

		Mesh(
			std::vector<Vertex> const& vertices,
			std::vector<std::shared_ptr<Texture2D>> const& textures,
			std::vector<GLuint> indices,
			BufferLayout layout,
			VertexBuffer::EUsage usage
		);

		void draw(ShaderProgram& shader);
	private:
		VertexBuffer VBO;
		VertexArray VAO;
		IndexBuffer IBO;

	};






}


