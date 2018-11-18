#include "Environment.h"

Environment::Environment() {
	circleColor.g = 140;
	circleColor.a = 255;

	circlePos = new glm::vec2[circleCount];
	circleVel = new glm::vec2[circleCount];
};

Environment::~Environment() = default;

