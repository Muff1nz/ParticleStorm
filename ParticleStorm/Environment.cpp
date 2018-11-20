#include "Environment.h"
#include <iostream>

Environment::Environment() {
	circleColor.g = 140;
	circleColor.a = 255;

	circlePos = new glm::vec2[circleCount];
	circleVel = new glm::vec2[circleCount];
};

Environment::~Environment() = default;

//Maybe optimize with XOR swapping?
void Environment::SwapParticles(const int one, const int two) const {
	const auto tempPos = circlePos[one];
	const auto tempVel = circleVel[one];
	circlePos[one] = circlePos[two];
	circleVel[one] = circleVel[two];
	circlePos[two] = tempPos;
	circleVel[two] = tempVel;
}

