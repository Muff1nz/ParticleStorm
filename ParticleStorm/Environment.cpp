#include "Environment.h"
#include <iostream>


Environment::Environment() {
	Init();
}

Environment::Environment(int circleCount, int circleRadius, int seed) : circleCount(circleCount), circleRadius(circleRadius) {
	Init();
	this->seed = seed;
}

Environment::~Environment() = default;

void Environment::Init() {
	circleColor.g = 140;
	circleColor.a = 255;

	circlePos = new glm::vec2[circleCount];
	circleVel = new glm::vec2[circleCount];

	renderOrder = new int[circleCount];
	for (int i = 0; i < circleCount; ++i) {
		renderOrder[i] = i;
	}
}


//Maybe optimize with XOR swapping?
void Environment::SwapParticles(const int one, const int two) const {
	const auto tempPos = circlePos[one];
	circlePos[one] = circlePos[two];
	circlePos[two] = tempPos;

	const int tempi = renderOrder[one];
	renderOrder[one] = renderOrder[two];
	renderOrder[two] = tempi;

	const auto tempVel = circleVel[one];
	circleVel[one] = circleVel[two];
	circleVel[two] = tempVel;
}

