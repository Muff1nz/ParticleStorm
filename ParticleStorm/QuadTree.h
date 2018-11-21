#pragma once
#include "Environment.h"
#include "Rect.h"
#include <string>

class Environment;

class QuadTree {
public:
	struct Stats {
		int swapCount;
		int overflowCount;
		int leafQuadCount;

		std::string ToString() {
			return "QuadTree stats: SwapCount: " + std::to_string(swapCount) +
				" OverflowCount: " + std::to_string(overflowCount) +
				" leafQuatCount: " + std::to_string(leafQuadCount);
		}

		void clear() {
			swapCount = 0;
			overflowCount = 0;
			leafQuadCount = 0;
		}
	};

	QuadTree(Environment* environment, Rect rect_);
	~QuadTree();
	QuadTree** subTree = nullptr;
	Rect rect;
	std::vector<int> overflow;
	int start{}, end{};

	void Build(QuadTree* parent, int& current, Stats& stats);
private:
	Environment* environment;
	const int radiusSquared;
	const int maxParticles = 50;
	Rect paddedRect;
	QuadTree** secretSubTree;
	
	bool ParticleBoxCollision(const glm::vec2& center, const Rect& rect) const;


};

