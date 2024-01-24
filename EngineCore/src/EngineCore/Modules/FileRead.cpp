#include <fstream>

#include "FileRead.hpp"
#include "EngineCore/Logs.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace EngineCore {

	unsigned char* read_image(const char* path, int &width, int& height, int &channels) {
		return stbi_load(path, &width, &height, &channels, 0);
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
		else {
			LOG_CRITICAL("[FILE READ] Failed to read file '{}'", name);
		}
	};
}