#pragma once
#include "Environment.h"
#include "Rect.h"
#include "Stats.h"

class Environment;

class QuadTree {
public:
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
	
	bool ParticleBoxCollision(const glm::vec2& circleCenter, const Rect& rect) const;
};

