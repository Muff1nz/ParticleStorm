#pragma once
#include "Environment.h"
#include "Rect.h"
#include "Stats.h"
#include "ConcurrentVector.h"

class Environment;

class QuadTree {
public:
	QuadTree(Environment* environment, Rect rect_);
	~QuadTree();

	void Build(ConcurrentVectror<QuadTree*>* quads, Stats* stats);

	const int maxParticles = 100;

	int start{}, end{};
	std::vector<int> overflow;

	Rect rect;
	QuadTree** subTree = nullptr;

	const int maxDepth = 10;
	int depth = 0;

private:
	const int radiusSquared;

	Rect paddedRect;
	QuadTree** secretSubTree{};

	Environment* environment;


	void Build(QuadTree* parent, int& current, ConcurrentVectror<QuadTree*>* quads, Stats* stats);
	bool QuadLimitReached();
	bool ParticleBoxCollision(const glm::vec2& circleCenter, const Rect& rect) const;
	void CreateSubTrees(ConcurrentVectror<QuadTree*>* quads, Stats* stats);
	void DestroySubTrees();
	void SwapCallback(int one, int two, int overflow);
	void PopulateQuadTreeWithParticles(QuadTree* parent, int& current, Stats* stats);
	void BuildSubTrees(ConcurrentVectror<QuadTree*>* quads, Stats* stats);

	bool operator== (const QuadTree &other) const;
	bool operator< (const QuadTree &other) const;
	bool operator> (const QuadTree &other) const;
};

