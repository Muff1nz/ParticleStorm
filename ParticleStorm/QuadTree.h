#pragma once
#include "Environment.h"
#include "Rect.h"

class Environment;

class QuadTree {
public:
	QuadTree(QuadTree* parent, Environment* environment, Rect rect_);
	~QuadTree();
	int QuadSize();

	void BuildRoot();

	const int maxParticles = 100;
	const int minParticlesPerThread = 400;
	const int maxParticlesPerThread = 1600;
	const int maxDepth = 10;
	int depth;

	std::vector<int> particlesInQuad;

	std::vector<int> internalParticle;
	std::vector<glm::vec2> internalParticlePos;
	std::vector<int> externalParticle;

	Rect rect;

	QuadTree* parent = nullptr;
	QuadTree** subTree = nullptr;

private:
	const int radiusSquared;

	Rect paddedRect;
	QuadTree** secretSubTree{};

	Environment* environment;

	void Build();
	bool QuadLimitReached();
	bool ParticleBoxCollision(const glm::vec2& circleCenter, const Rect& rect) const;
	void CreateSubTrees();
	void PopulateQuadTreeWithParticles(const int start, const int end);
	void BuildSubTrees() const;
	void BuildSubTreesThreaded() const;
};

