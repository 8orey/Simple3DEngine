#pragma once

#include <vector>

namespace EngineCore {
	using uint32_t = unsigned int;

	enum class ShaderDataType {
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
	};

	struct BufElement {
		ShaderDataType type;
		uint32_t component_type;
		size_t components_cnt;
		size_t size;
		size_t offset;

		BufElement(const ShaderDataType type);
	};

	class BufferLayout {
	public:
		BufferLayout() = default;
		BufferLayout(std::initializer_list<BufElement> elements) 
			: m_elements(std::move(elements))
		{
			size_t offset = 0;
			m_stride = 0;
			for (auto& elem : m_elements) {
				elem.offset = offset;
				offset += elem.size;
				m_stride += elem.size;
			}
		}

		const std::vector<BufElement>& get_elements() const { return m_elements; }
		size_t get_stride() const { return m_stride; }
		
	private:
		std::vector<BufElement> m_elements;
		size_t m_stride;
	};

	
	class VertexBuffer {
	public:
		enum class EUsage {
			Static,
			Dynamic,
			Stream,
		};

		VertexBuffer(const void* data, const size_t size, BufferLayout buf_layout, const EUsage usage = VertexBuffer::EUsage::Static);
		~VertexBuffer();

		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer& operator=(VertexBuffer&& vertexBuffer) noexcept;
		VertexBuffer(VertexBuffer&& vertexBuffer) noexcept;

		void bind() const;
		static void unbind();

		const BufferLayout& get_layout() const { return m_buffer_layout; }

	private:
		uint32_t m_id = 0;
		BufferLayout m_buffer_layout;
	};
}