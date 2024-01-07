

namespace EngineCore {
	using uint32_t = unsigned int;

	class Texture2D {
	public:

		Texture2D(const unsigned char* data, const uint32_t width, const uint32_t height);
		~Texture2D();

		Texture2D(const Texture2D&) = delete;
		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D& operator=(Texture2D&& texture) noexcept;
		Texture2D(Texture2D&& texture) noexcept;

		void bind(const uint32_t unit) const;

	private:
		uint32_t m_id = 0, m_width = 0, m_height = 0;
	};



}