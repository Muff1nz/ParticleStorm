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
	const int maxParticles = 100;

	//QuadTree** subTree = nullptr;

	//Rect rect;

	bool ParticleBoxCollision(const glm::vec2& center, const Rect& rect) const;
	//void Build(int parentStart, int parentEnd, int myStart, int& outEnd, bool root);
	//Rect

	//The particles contained in the quad tree are between the start and end index in the particles arrays
	//When building the quad tree, rearrange the big particle arrays such that particles inside one quad lay
	//	sequentially inside the array between start and end index
	//int start index, end index

	//Particle overflow (A set of pointers pointing to particles that are also in other quads


};

