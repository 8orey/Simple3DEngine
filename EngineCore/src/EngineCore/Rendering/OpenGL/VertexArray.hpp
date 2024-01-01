#pragma once

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

namespace EngineCore {
	using uint32_t = unsigned int;

	class VertexArray {
	public:
		VertexArray();
		~VertexArray();

		VertexArray(const VertexArray&) = delete;
		VertexArray& operator=(const VertexArray&) = delete;
		VertexArray& operator=(VertexArray&& vertexArray) noexcept;
		VertexArray(VertexArray&& vertexArray) noexcept;

		void add_vertex_buffer(const VertexBuffer& vertex_buffer);
		void set_index_buffer(const IndexBuffer& index_buffer);
		void bind() const;
		static void unbind();
		size_t get_indicies_count() const { return m_indicies_count; };

	private:
		uint32_t m_id = 0;
		uint32_t m_elements_count = 0;
		size_t m_indicies_count = 0;
	};
}