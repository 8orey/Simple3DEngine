#include <fstream>
#include <memory>

#include "FileRead.hpp"
#include "EngineCore/Logs.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION 1
#include <stb_image.h>

namespace EngineCore {

	Image_t::~Image_t() {
		free(this->image);
	}

	Image_t::Image_t(unsigned char* img, int w, int h, int c, Image_t::format fmt):
		image(img), width(w), height(h), channels(c), fmt(fmt)
	{}

	Image_t read_image(const char* path) {
		int width, height, channels;
		unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
		LOG_INFO("[IMAGE DATA] size = {}x{}x{} | path = {}", width, height, channels, path);
		if (data == NULL) {
			LOG_ERROR("READ_IMAGE_ERROR: Failed read from path: {}", path);
		}

		return { data, width, height, channels, (channels == 3 ? Image_t::format::JPEG : Image_t::format::PNG)};
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

	static Assimp::Importer importer;

	const aiScene* import_scene(std::string const& path, uint32_t flags) {
		auto scene = importer.ReadFile(path.c_str(), flags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			LOG_CRITICAL("ASSIMP [IMPORT SCENE ERROR]: {}", importer.GetErrorString());
			return nullptr;
		}

		return scene;
	}



}