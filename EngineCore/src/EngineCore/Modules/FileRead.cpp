#include <fstream>
#include <memory>

#include "FileRead.hpp"
#include "EngineCore/Logs.hpp"

#define STB_IMAGE_IMPLEMENTATION 1
#include <stb_image.h>

namespace EngineCore {

	Image_t::~Image_t() {
		free(this->image);
	}

	Image_t::Image_t(unsigned char* img, int w, int h, int c) :
		image(img), width(w), height(h), channels(c)
	{}

	Image_t read_image(const char* path) {
		int width, height, channels;
		unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
		return { data, width, height, channels };
	}

	std::string read_file(const std::string& name) {
		std::ifstream input(name);
		if (input.is_open()) {
			std::string res, buf;
			while (std::getline(input, buf)) {
				res += buf + '\n';
			}
			return std::move(res); 
		}
		LOG_CRITICAL("[FILE READ] Failed to read file '{}'", name);
		return "";
	};




}