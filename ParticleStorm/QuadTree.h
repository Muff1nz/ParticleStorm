#pragma once
#include "Environment.h"
#include "Rect.h"
#include "Stats.h"

class Environment;

class QuadTree {
public:
	QuadTree(Environment* environment, Rect rect_);
	~QuadTree();
	bool QuadLimitReached();
	std::vector<QuadTree> Build(Stats& stats);

	const int maxParticles = 100;

	int start{}, end{};
	std::vector<int> overflow;

	Rect rect;
	QuadTree** subTree = nullptr;

	void Build(QuadTree* parent, int& current, std::vector<QuadTree>& quads, Stats& stats);
private:
	const int radiusSquared;

	Rect paddedRect;
	QuadTree** secretSubTree{};

	Environment* environment;

	bool ParticleBoxCollision(const glm::vec2& circleCenter, const Rect& rect) const;
	void BuildSubTrees(std::vector<QuadTree>& quads, Stats& stats);
	void DestroySubTrees();
	void PopulateQuadTreeWithParticles(QuadTree* parent, int& current, Stats& stats);

	bool operator== (const QuadTree &other) const;
	bool operator< (const QuadTree &other) const;
	bool operator> (const QuadTree &other) const;
};

