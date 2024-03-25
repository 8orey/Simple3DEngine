#pragma once

#include "EngineCore/Modules/FileRead.hpp"

namespace EngineCore {
	using uint32_t = unsigned int;

	class Texture2D {
	public:

		enum type : char {
			ambient = 'a',
			diffuse = 'd',
			specular = 's',
			none = 'n',
		};

		Texture2D(Image_t const& img, type t);
		~Texture2D();

		Texture2D(const Texture2D&) = delete;
		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D& operator=(Texture2D&& texture) noexcept;
		Texture2D(Texture2D&& texture) noexcept;

		void bind(const uint32_t unit) const;
		uint32_t get_id() const {
			return m_id;
		};
		type get_type() const {
			return m_type;
		}

		void free() {
			m_id = 0;
			m_width = 0;
			m_height = 0;
			m_type = Texture2D::type::none;
		}

	private:
		uint32_t m_id = 0;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		type m_type;
	};



}