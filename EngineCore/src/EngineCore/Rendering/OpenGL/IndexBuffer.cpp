#include "IndexBuffer.hpp"
#include "EngineCore/Logs.hpp" 

#include <glad/glad.h>

namespace EngineCore {

	constexpr GLenum usage_to_GLenum(const VertexBuffer::EUsage usage) {

		switch (usage) {
		case VertexBuffer::EUsage::Static: return GL_STATIC_DRAW;
		case VertexBuffer::EUsage::Dynamic: return GL_DYNAMIC_DRAW;
		case VertexBuffer::EUsage::Stream: return GL_STREAM_DRAW;
		}

		LOG_ERROR("Unknown VertexBuffer usage");
		return GL_STREAM_DRAW;
	}

	IndexBuffer::IndexBuffer(const std::vector<GLuint>& data, const VertexBuffer::EUsage usage)
		: m_count(data.size()) {
		glGenBuffers(1, &m_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(GLuint), data.data(), usage_to_GLenum(usage));
	}

	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &m_id);
	}

	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& index_buf) noexcept {
		m_id = index_buf.m_id;
		m_count = index_buf.m_count;
		index_buf.m_id = 0;
		index_buf.m_count = 0;
		return *this;
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& index_buf) noexcept {
		m_id = index_buf.m_id;
		m_count = index_buf.m_count;
		index_buf.m_id = 0;
		index_buf.m_count = 0;
	}

	void IndexBuffer::bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	void IndexBuffer::unbind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}