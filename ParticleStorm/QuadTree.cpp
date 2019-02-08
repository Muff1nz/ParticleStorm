#include "QuadTree.h"
#include "Stats.h"
#include <iostream>

QuadTree::QuadTree(QuadTree* parent, Environment* environment, Rect rect_) : radiusSquared(pow(environment->particleRadius, 2)) {
	this->parent = parent;
	this->environment = environment;
	const int r = environment->particleRadius * 1.5f;
	rect = rect_;
	paddedRect = Rect(rect_.x - r, rect_.y - r, rect_.w + r, rect_.h + r);
}

QuadTree::~QuadTree() {
	if (subTree != nullptr) {
		for (int i = 0; i < 4; ++i) {
			delete secretSubTree[i];
		}
	}
	delete subTree;
}

int QuadTree::QuadSize() {
	if (parent == nullptr)
		return environment->particleCount;
	return particlesInQuad.Size();
}

bool QuadTree::QuadLimitReached() { return QuadSize() >= maxParticles; }

void QuadTree::BuildRoot(ConcurrentVector<QuadTree*>* quads, Stats* stats) {
	int current = 0;
	quads->Clear();
	Build(&*quads, stats);
}

void QuadTree::Build(ConcurrentVector<QuadTree*>* quads, Stats* stats) {
	if (parent != nullptr) {
		if (parent->parent == nullptr)
			PopulateQuadTreeWithParticles(stats, 0, environment->particleCount);
		else
			PopulateQuadTreeWithParticles(stats, 0, parent->particlesInQuad.Size());
	}

	if (QuadLimitReached()) {
		CreateSubTrees(quads, stats);
	} else {
		subTree = nullptr;
		quads->push_back(this);
	}

	if (subTree == nullptr) {
		++stats->quadTreeLeafTotalLastSecond;
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


void QuadTree::PopulateQuadTreeWithParticles(Stats* stats, const int start, const int end) {
	const auto allParticles = environment->particlePos;

	particlesInQuad.Clear();
	if (parent->parent == nullptr) { //Parent is root node, containing all particles
		for (int i = start; i < end; i++) {
			if (ParticleBoxCollision(allParticles[i], paddedRect)) {
				particlesInQuad.Add(i);
			}
		}
	} else { //Parent contains a subset of all particles
		for (int i = start; i < end; i++) {
			if (ParticleBoxCollision(allParticles[parent->particlesInQuad[i]], paddedRect)) {
				particlesInQuad.Add(parent->particlesInQuad[i]);
			}
		}
	}
}

void QuadTree::BuildSubTrees(ConcurrentVector<QuadTree*>* quads, Stats* stats) const {
	for (int i = 0; i < 4; ++i) {
		subTree[i]->Build(quads, stats);
	}
}

void QuadTree::CreateSubTrees(ConcurrentVector<QuadTree*>* quads, Stats* stats) {
	if (subTree == nullptr) {
		if (secretSubTree != nullptr) {
			subTree = secretSubTree;
		} else {
			subTree = new QuadTree*[4];
			subTree[0] = new QuadTree(this, environment, Rect(rect.x, rect.y, rect.w / 2, rect.h / 2));
			subTree[1] = new QuadTree(this, environment, Rect(rect.x + rect.w / 2, rect.y, rect.w / 2, rect.h / 2));
			subTree[2] = new QuadTree(this, environment, Rect(rect.x, rect.y + rect.h / 2, rect.w / 2, rect.h / 2));
			subTree[3] = new QuadTree(this, environment, Rect(rect.w / 2 + rect.x, rect.y + rect.h / 2, rect.w / 2, rect.h / 2));
			secretSubTree = subTree;
		}
	}

	//environment->workerThreads.AddWork([=] { BuildSubTrees(quads, stats); });
	BuildSubTrees(quads, stats);
}
