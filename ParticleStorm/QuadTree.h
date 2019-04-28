#pragma once
#include "Environment.h"
#include "Rect.h"
#include "MultiIntVector.h"
#include "ConcurrentVector.h"

class QuadTree {
public:
	QuadTree(QuadTree* parent, Environment* environment, Rect rect_, ConcurrentVector<QuadTree*>* quads);
	~QuadTree();
	int QuadSize() const;

	void BuildRoot();

	QuadTree* tree;
	ConcurrentVector<QuadTree*>* quads;

	const int maxParticles = 100;
	const int particlesPerThreadLevel1 = 800;
	const int particlesPerThreadLevel2 = particlesPerThreadLevel1 * 4;
	const int particlesPerThreadLevel3 = particlesPerThreadLevel2 * 4;
	const int maxDepth = 10;
	int depth;

	int finishedThreads;

	std::vector<int> particlesInQuad;
	std::mutex QuadLock;
	MultiIntVector particlesInQuadThreaded;

	std::vector<int> internalParticle;
	std::vector<glm::vec2> internalParticlePos;
	std::vector<int> externalParticle;

	Rect rect;

	QuadTree* parent = nullptr;
	QuadTree** subTree = nullptr;

private:
	const int radiusSquared;

	QuadTree** secretSubTree{};

	Environment* environment;

	void Build();
	void HandleSubTrees();
	void BuildThreaded(int start, int end, int threadCount, int ThreadNumber);
	bool QuadLimitReached() const;
	bool ParticleBoxCollision(const glm::vec2& circleCenter, const Rect& rect) const;
	void BuildBigSubTreesThreaded();
	void CreateSubTrees();
	void PopulateQuadTreeWithParticles();
	void PopulateQuadTreeWithParticlesThreaded(int start, int end, int ThreadNumber);
	void BuildSubTrees() const;
	void BuildSubTreesThreaded() const;
};

