#pragma once
#include <vector>
#include <vec2.hpp>

class LinearQuad {
public:
	std::vector<int> externalParticle;
	std::vector<int> internalParticle;
	std::vector<glm::vec2> internalParticlePos;
	
	void Clear() {
		externalParticle.clear();
		internalParticle.clear();
		internalParticlePos.clear();
	}
};
