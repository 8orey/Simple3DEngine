#pragma once

#include <string>

namespace EngineCore {

	std::string read_file(const std::string& name);
	
	struct Image_t {
		unsigned char* image;
		int width;
		int height;
		int channels;

		Image_t(unsigned char* img, int w, int h, int c);

		Image_t(Image_t const&) = delete;
		auto operator=(Image_t const&) = delete;

		~Image_t();
	};

	Image_t read_image(const char* path);

}