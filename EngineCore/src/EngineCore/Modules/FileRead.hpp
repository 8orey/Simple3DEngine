#pragma once

#include <string>

#include <assimp/scene.h>

namespace EngineCore {

	std::string read_file(const std::string& name);
	
	struct Image_t {

		enum format: char  {
			PNG, JPEG
		};

		unsigned char* image;
		Image_t::format fmt;
		int width;
		int height;
		int channels;

		Image_t(unsigned char* img, int w, int h, int c, Image_t::format fmt);
		~Image_t();

		Image_t(Image_t const&) = delete;
		auto operator=(Image_t const&) = delete;
		

	};

	Image_t read_image(const char* path);

	const aiScene* import_scene(std::string const& path, uint32_t flags);


}