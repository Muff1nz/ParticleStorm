#pragma once
#include <string>
#include <vec2.hpp>
#include <mat3x3.hpp>

class Utils {
public:
	static std::string VecToString(glm::vec2 vec) {
		return "(x: " + std::to_string(vec.x) + " y: " + std::to_string(vec.y) + ")";
	}

	static std::string Mat3ToString(glm::mat3 mat) {
		std::string str;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				str += std::to_string(mat[i][j]) + " ";
			}
			str += '\n';
		}
		return str;
	}

	static glm::mat3 ChangeOrder(glm::mat3 mat) {
		glm::mat3 result;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {		
				result[i][j] = mat[j][i];
			}
		}
		return result;
	}
};
