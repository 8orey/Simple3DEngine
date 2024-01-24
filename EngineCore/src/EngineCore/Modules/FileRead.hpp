#include <string>

namespace EngineCore {
	std::string read_file(const std::string& name);
	
	unsigned char* read_image(const char* path, int& width, int& height, int& channels);

}