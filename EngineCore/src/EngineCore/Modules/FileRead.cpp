#include <string>
#include <fstream>
#include "FileRead.hpp"
#include "EngineCore/Logs.hpp"

namespace EngineCore {
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