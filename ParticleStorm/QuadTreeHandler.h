#pragma once

#include "QuadTree.h"
#include "LinearQuad.h"
#include "CollisionChecker.h"
#include "PhysicsParticlesEntity.h"
#include "Range.h"
#include "WorkerThreadPool.h"
#include "Stats.h"
#include "WorldEntity.h"


class QuadTreeHandler {
public:
	QuadTreeHandler(WorkerThreadPool* workerThreads, Stats* stats);
	~QuadTreeHandler();

	void Init(PhysicsParticlesEntity* particles, WorldEntity* world);
	void BuildLinearQuadTree(std::vector<LinearQuad*>* linearQuads, std::vector<Range>& quadSections);
	void PopulateQuadData();

private:
	PhysicsParticlesEntity* particles;
	WorldEntity* world;

	WorkerThreadPool* workerThreads;
	Stats* stats;

	QuadTree* tree;
	ConcurrentVector<QuadTree*> quads;

	const int maxParticles = 100;
	const int maxDepth = 10;
	const int particlesPerThreadLevel1 = 800;
	const int particlesPerThreadLevel2 = particlesPerThreadLevel1 * 4;
	const int particlesPerThreadLevel3 = particlesPerThreadLevel2 * 4;
	CollisionChecker collisionChecker;

	void ResizeLinearQuads(std::vector<LinearQuad*>* linearQuads);
	void CalculateLinearQuads(std::vector<LinearQuad*>* linearQuads, int start, int end);

	void BuildQuadTree();
	void Build(QuadTree* tree);
	void BuildThreaded(QuadTree* tree, int start, int end, int threadCount, int ThreadNumber);

	void PopulateSubTrees(QuadTree* tree);
	void PopulateSubTreesNotThreaded(QuadTree* tree);
	void PopulateSubTreesThreaded(QuadTree* tree);
	void PopulateBigSubTreesThreaded(QuadTree* tree);

	void PopulateQuadTreeWithParticles(QuadTree* tree) const;
	void PopulateQuadTreeWithParticlesThreaded(QuadTree* tree, int start, int end, int threadNumber) const;

	void HandleSubTrees(QuadTree* tree);

	bool QuadLimitReached(QuadTree* tree) const;
};

