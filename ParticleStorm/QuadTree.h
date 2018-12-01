#pragma once
#include "Environment.h"
#include "Rect.h"
#include "Stats.h"

class Environment;

class QuadTree {
public:
	QuadTree(Environment* environment, Rect rect_);
	~QuadTree();

	const int maxParticles = 100;

	int start{}, end{};
	std::vector<int> overflow;

	Rect rect;
	QuadTree** subTree = nullptr;

	void Build(QuadTree* parent, int& current, Stats& stats);

private:
	const int radiusSquared;

	Rect paddedRect;
	QuadTree** secretSubTree{};

	Environment* environment;

	bool ParticleBoxCollision(const glm::vec2& circleCenter, const Rect& rect) const;
};

