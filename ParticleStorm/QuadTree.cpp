#include "QuadTree.h"
#include "Stats.h"
#include <iostream>

QuadTree::QuadTree(QuadTree* parent, Environment* environment, Rect rect_, ConcurrentVector<QuadTree*>* quads) : radiusSquared(pow(environment->particleRadius, 2)) {
	this->parent = parent;
	this->environment = environment;
	this->quads = quads;

	const int r = environment->particleRadius * 2.0f;
	rect = rect_;
	paddedRect = Rect(rect_.x - r, rect_.y - r, rect_.w + r, rect_.h + r);

	if (parent != nullptr)
		depth = parent->depth + 1;
	else
		depth = 0;
}

QuadTree::~QuadTree() {
	if (subTree != nullptr) {
		for (int i = 0; i < 4; ++i) {
			delete secretSubTree[i];
		}
	}
	delete subTree;
}

int QuadTree::QuadSize() const {
	if (parent == nullptr)
		return environment->particleCount;
	return particlesInQuad.size();
}

bool QuadTree::QuadLimitReached() const { return QuadSize() >= maxParticles && depth <= maxDepth; }

void QuadTree::BuildRoot() {
	quads->clear();
	HandleSubTrees();
}

void QuadTree::Build() {
	PopulateQuadTreeWithParticles();
	HandleSubTrees();
}

void QuadTree::BuildThreaded(const int start, const int end, const int threadCount, const int ThreadNumber) {
	PopulateQuadTreeWithParticlesThreaded(start, end, ThreadNumber);

	QuadLock.lock();
	++finishedThreads;
	if (finishedThreads != threadCount) {
		QuadLock.unlock();
		return;
	}
	QuadLock.unlock();

	particlesInQuad = particlesInQuadThreaded.Merge();

	HandleSubTrees();
}

void QuadTree::HandleSubTrees() {
	if (QuadLimitReached()) {
		CreateSubTrees();
	} else {
		subTree = nullptr;
		quads->push_back(this);
		for (int i : particlesInQuad) {
			++environment->particleQuadCount[i];
		}
	}
}

bool QuadTree::ParticleBoxCollision(const glm::vec2& circleCenter, const Rect& rect) const {
	const auto radius = environment->particleRadius;
	glm::vec2 circleDistance;

	circleDistance.x = abs(circleCenter.x - rect.centerX);
	circleDistance.y = abs(circleCenter.y - rect.centerY);

	if (circleDistance.x > rect.halfW + radius)  return false; 
	if (circleDistance.y > rect.halfH)  return false; 

	if (circleDistance.x <= rect.halfW)  return true; 
	if (circleDistance.y <= rect.halfH)  return true; 

	const auto cornerDistanceSq = 
		pow(circleDistance.x - rect.halfW, 2) +
		pow(circleDistance.y - rect.halfH, 2);

	return cornerDistanceSq <= radiusSquared;
}

void QuadTree::PopulateQuadTreeWithParticles() {
	const int size = parent->QuadSize();
	const auto allParticles = environment->particlePos;
	particlesInQuad.clear();
	if (parent->parent == nullptr) { //Parent is root node, containing all particles
		for (int i = 0; i < size; i++) {
			if (ParticleBoxCollision(allParticles[i], paddedRect)) {
				particlesInQuad.push_back(i);
			}
		}
	} else { //Parent contains a subset of all particles
		for (int i = 0; i < size; i++) {
			if (ParticleBoxCollision(allParticles[parent->particlesInQuad[i]], paddedRect)) {
				particlesInQuad.push_back(parent->particlesInQuad[i]);
			}
		}
	}
}

void QuadTree::PopulateQuadTreeWithParticlesThreaded(const int start, const int end, const int ThreadNumber) {
	const auto allParticles = environment->particlePos;
	if (parent->parent == nullptr) { //Parent is root node, containing all particles
		for (int i = start; i < end; i++) {
			if (ParticleBoxCollision(allParticles[i], paddedRect)) {
				particlesInQuadThreaded.Add(i, ThreadNumber);
			}
		}
	} else { //Parent contains a subset of all particles
		for (int i = start; i < end; i++) {
			if (ParticleBoxCollision(allParticles[parent->particlesInQuad[i]], paddedRect)) {
				particlesInQuadThreaded.Add(parent->particlesInQuad[i], ThreadNumber);
			}
		}
	}
}

void QuadTree::BuildSubTrees() const {
	for (int i = 0; i < 4; ++i) {
		subTree[i]->Build();
	}
}

void QuadTree::BuildSubTreesThreaded() const {
	for (int i = 0; i < 4; ++i) {
		environment->workerThreads.AddWork([=] { subTree[i]->Build(); });
	}
}

void QuadTree::BuildBigSubTreesThreaded() {
	int threads = QuadSize() / particlesPerThreadLevel2;
	if (threads > environment->workerThreadCount)
		threads = environment->workerThreadCount;
	if (threads < 2)
		threads = 2;

	std::vector<Range> partitions;
	environment->workerThreads.PartitionForWorkers(QuadSize(), partitions, threads);

	for (int i = 0; i < 4; ++i) {
		subTree[i]->particlesInQuad.clear();
		subTree[i]->particlesInQuadThreaded.Reset(threads);
		subTree[i]->finishedThreads = 0;

		for (int j = 0; j < partitions.size(); ++j) {
			environment->workerThreads.AddWork([=] { subTree[i]->BuildThreaded(partitions[j].lower, partitions[j].upper, threads, j); });
		}
	
	}
}

void QuadTree::CreateSubTrees() {
	if (subTree == nullptr) {
		if (secretSubTree != nullptr) {
			subTree = secretSubTree;
		} else {
			subTree = new QuadTree*[4];
			subTree[0] = new QuadTree(this, environment, Rect(rect.x, rect.y, rect.w / 2, rect.h / 2), quads);
			subTree[1] = new QuadTree(this, environment, Rect(rect.x + rect.w / 2, rect.y, rect.w / 2, rect.h / 2), quads);
			subTree[2] = new QuadTree(this, environment, Rect(rect.x, rect.y + rect.h / 2, rect.w / 2, rect.h / 2), quads);
			subTree[3] = new QuadTree(this, environment, Rect(rect.w / 2 + rect.x, rect.y + rect.h / 2, rect.w / 2, rect.h / 2), quads);
			secretSubTree = subTree;
		}
	}

	if (QuadSize() >= particlesPerThreadLevel3)
		BuildBigSubTreesThreaded();
	else if (QuadSize() >= particlesPerThreadLevel2)
		BuildSubTreesThreaded();
	else 
		BuildSubTrees();
}
