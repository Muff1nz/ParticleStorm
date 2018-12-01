#pragma once
#include <string>
#include <vec2.hpp>

class Utils {
public:
	static std::string vecToString(glm::vec2 vec) {
		return "(x: " + std::to_string(vec.x) + " y: " + std::to_string(vec.y) + ")";
	}
};
