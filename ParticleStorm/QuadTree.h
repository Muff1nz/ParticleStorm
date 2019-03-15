#pragma once
#include "Environment.h"
#include "Rect.h"
#include "Stats.h"
#include "ConcurrentVector.h"
#include "LazyVector.h"

class Environment;

class QuadTree {
public:
	QuadTree(QuadTree* parent, Environment* environment, Rect rect_);
	~QuadTree();
	int QuadSize();

	void BuildRoot(ConcurrentVector<QuadTree*>* quads, Stats* stats);

	const int maxParticles = 100;
	const int minParticlesPerThread = 400;
	const int maxParticlesPerThread = 1600;
	LazyVector<int> particlesInQuad{};

	Rect rect;

	QuadTree* parent = nullptr;
	QuadTree** subTree = nullptr;

private:
	const int radiusSquared;

	Rect paddedRect;
	QuadTree** secretSubTree{};

	Environment* environment;

	void Build(ConcurrentVector<QuadTree*>* quads, Stats* stats);
	bool QuadLimitReached();
	bool ParticleBoxCollision(const glm::vec2& circleCenter, const Rect& rect) const;
	void CreateSubTrees(ConcurrentVector<QuadTree*>* quads, Stats* stats);
	void PopulateQuadTreeWithParticles(Stats* stats, const int start, const int end);
	void BuildSubTrees(ConcurrentVector<QuadTree*>* quads, Stats* stats) const;
	void BuildSubTreesThreaded(ConcurrentVector<QuadTree*>* quads, Stats* stats) const;
};

