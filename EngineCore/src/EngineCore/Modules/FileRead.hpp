#pragma once

#include <string>

#include <assimp/scene.h>

namespace EngineCore {

	std::string read_file(const std::string& name);
	
	enum image_format {
		PNG, JPEG
	};

	struct Image_t {
		unsigned char* image;
		image_format f;
		int width;
		int height;
		int channels;

		Image_t(unsigned char* img, int w, int h, int c, image_format format);

		Image_t(Image_t const&) = delete;
		auto operator=(Image_t const&) = delete;

		~Image_t();
	};

	Image_t read_image(const char* path);

	const aiScene* import_scene(std::string const& path, uint32_t flags);


}