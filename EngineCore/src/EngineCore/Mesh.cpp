#include "EngineCore/Mesh.hpp"



namespace EngineCore {

	Mesh::Mesh(
		const std::vector<Vertex>& vertices,
		const std::vector<GLuint>& indices,
		const std::vector<Texture2D> &texture
	): 
		vertices(vertices),
		indicies(indices),
		texture(texture),
		VBO(vertices, 
			BufferLayout{
				ShaderDataType::Float3,
				ShaderDataType::Float3,
				ShaderDataType::Float2,
			}),
		VAO(),
		IBO(indices, VertexBuffer::EUsage::Dynamic)
	{
		VAO.bind();
		VAO.add_vertex_buffer(VBO);
		VAO.set_index_buffer(IBO);
	}

	void Mesh::Draw(
		const ShaderProgram& shader,
		const Camera& camera
	) {
		shader.bind();
		VAO.bind();



	}

}