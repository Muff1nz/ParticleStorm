#include "QuadTree.h"
#include "Stats.h"
#include <iostream>

QuadTree::QuadTree(QuadTree* parent, Environment* environment, Rect rect_) : radiusSquared(pow(environment->particleRadius, 2)) {
	this->parent = parent;
	this->environment = environment;
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

int QuadTree::QuadSize() {
	if (parent == nullptr)
		return environment->particleCount;
	return particlesInQuad.size();
}

bool QuadTree::QuadLimitReached() { return QuadSize() >= maxParticles && depth <= maxDepth; }

void QuadTree::BuildRoot() {
	int current = 0;
	environment->quads.Clear();
	Build();
}

void QuadTree::Build() {
	if (parent != nullptr) {
		if (parent->parent == nullptr)
			PopulateQuadTreeWithParticles(0, environment->particleCount);
		else
			PopulateQuadTreeWithParticles(0, parent->particlesInQuad.size());
	}

	if (QuadLimitReached()) {
		CreateSubTrees();
	} else {
		subTree = nullptr;
		environment->quads.Push(this);
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


void QuadTree::PopulateQuadTreeWithParticles(const int start, const int end) {
	const auto allParticles = environment->particlePos;

	particlesInQuad.clear();
	if (parent->parent == nullptr) { //Parent is root node, containing all particles
		for (int i = start; i < end; i++) {
			if (ParticleBoxCollision(allParticles[i], paddedRect)) {
				particlesInQuad.push_back(i);
			}
		}
	} else { //Parent contains a subset of all particles
		for (int i = start; i < end; i++) {
			if (ParticleBoxCollision(allParticles[parent->particlesInQuad[i]], paddedRect)) {
				particlesInQuad.push_back(parent->particlesInQuad[i]);
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
	for (int i = 0; i < 4; ++i) { //TODO: Sub-thread individual quads too.
		environment->workerThreads.AddWork([=] { subTree[i]->Build(); });
	}
}

void QuadTree::CreateSubTrees() {
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

	if (particlesInQuad.size() >= maxParticlesPerThread)
		BuildSubTreesThreaded();
	else if (particlesInQuad.size() >= minParticlesPerThread)
		environment->workerThreads.AddWork([=] { BuildSubTrees(); });
	else
		BuildSubTrees();
}
