#include <glad/glad.h>
#include <algorithm>
#include <cmath>
#include "Texture2D.hpp"

namespace EngineCore {

	using uint32_t = unsigned int;
	Texture2D::Texture2D(const unsigned char* data, const uint32_t width, const uint32_t height) 
		: m_width(width)
		, m_height(height)
	{
        const GLsizei mip_levels = static_cast<GLsizei>(log2(std::max(width, height))) + 1;
        glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
        glTextureStorage2D(m_id, mip_levels, GL_RGB8, width, height);
        glTextureSubImage2D(m_id, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateTextureMipmap(m_id);
	}

    Texture2D::~Texture2D() {
        glDeleteTextures(1, &m_id);
    }

    Texture2D& Texture2D::operator=(Texture2D&& texture) noexcept {
        glDeleteTextures(1, &m_id);
        m_id = texture.m_id;
        m_width = texture.m_width;
        m_height = texture.m_height;
        texture.m_id = 0;
        texture.m_width = 0;
        texture.m_height = 0;
        return *this;
    }

    Texture2D::Texture2D(Texture2D&& texture) noexcept {
        glDeleteTextures(1, &m_id);
        m_id = texture.m_id;
        m_width = texture.m_width;
        m_height = texture.m_height;
        texture.m_id = 0;
        texture.m_width = 0;
        texture.m_height = 0;
    }

    void Texture2D::bind(const uint32_t unit) const {
        glBindTextureUnit(unit, m_id);
    }

}
