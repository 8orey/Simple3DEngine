#include <glad/glad.h>
#include <algorithm>
#include <cmath>
#include "Texture2D.hpp"
#include "EngineCore/Logs.hpp"

namespace EngineCore {

	using uint32_t = unsigned int;

    Texture2D::Texture2D(Image_t const& img, Texture2D::type t):
        m_width(img.width), 
        m_height(img.height),
        m_type(t)
	{
        const GLsizei mip_levels = static_cast<GLsizei>(log2(std::max(img.width, img.height))) + 1;
        glCreateTextures(GL_TEXTURE_2D, 1, &m_id);

        if (img.fmt == Image_t::format::JPEG) {
            glTextureStorage2D(m_id, mip_levels, GL_RGB8, img.width, img.height);
            glTextureSubImage2D(m_id, 0, 0, 0, img.width, img.height, GL_RGB, GL_UNSIGNED_BYTE, img.image);
        }
        if (img.fmt == Image_t::format::PNG) {
            //(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
            glTextureStorage2D(m_id, mip_levels, GL_RGBA8, img.width, img.height);
            //(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
            glTextureSubImage2D(m_id, 0, 0, 0, img.width, img.height, GL_RGBA, GL_UNSIGNED_BYTE, img.image);
        }


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
        m_type = texture.m_type;
        texture.m_id = 0;
        texture.m_width = 0;
        texture.m_height = 0;
        texture.m_type = Texture2D::type::none;
        return *this;
    }

    Texture2D::Texture2D(Texture2D&& texture) noexcept {
        glDeleteTextures(1, &m_id);
        m_id = texture.m_id;
        m_width = texture.m_width;
        m_height = texture.m_height;
        m_type = texture.m_type;
        texture.m_id = 0;
        texture.m_width = 0;
        texture.m_height = 0;
        texture.m_type = Texture2D::type::none;
    }

    void Texture2D::bind(const uint32_t unit) const {
        glBindTextureUnit(unit, m_id);
    }

}
