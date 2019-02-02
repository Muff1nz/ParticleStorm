#include "QuadTree.h"
#include "Stats.h"
#include <iostream>

QuadTree::QuadTree(Environment* environment, Rect rect_) : radiusSquared(pow(environment->particleRadius, 2)) {
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

bool QuadTree::QuadLimitReached() { return end - start >= maxParticles; }


std::vector<QuadTree> QuadTree::Build(Stats& stats) {
	int current = 0;
	std::vector<QuadTree> quads;
	Build(nullptr, current, quads, stats);
	return quads;
}

void QuadTree::Build(QuadTree* parent, int& current, std::vector<QuadTree> &quads, Stats& stats) {
	PopulateQuadTreeWithParticles(parent, current, stats);

	if (QuadLimitReached()) {
		BuildSubTrees(quads, stats);
	} else {
		DestroySubTrees();
		quads.push_back(*this);
	}

	if (subTree == nullptr) {
		++stats.quadTreeLeafTotalLastSecond;
		stats.quadTreeOverflowTotalLastSecond += overflow.size();
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


void QuadTree::PopulateQuadTreeWithParticles(QuadTree* parent, int& current, Stats& stats) {
	const auto particles = environment->particlePos;
	overflow.clear();
	if (parent != nullptr) {
		start = current;
		for (int i = parent->start; i < parent->end; i++) {
			if (ParticleBoxCollision(particles[i], paddedRect)) {
				if (i < current) {
					overflow.push_back(i);
				} else {
					if (i != current) {
						environment->SwapParticles(current, i);
						++stats.quadTreeSwapTotalLastSecond;
					}
					current++;
				}
			}
		}

		for (int i : parent->overflow) {
			if (ParticleBoxCollision(particles[i], paddedRect))
				overflow.push_back(i);
		}

		end = current;
	} else {
		start = 0;
		end = environment->particleCount;
	}
}

void QuadTree::BuildSubTrees(std::vector<QuadTree> &quads, Stats& stats) {
	if (subTree == nullptr) {
		if (secretSubTree != nullptr) {
			subTree = secretSubTree;
		} else {
			subTree = new QuadTree*[4];
			subTree[0] = new QuadTree(environment, Rect(rect.x, rect.y, rect.w / 2, rect.h / 2));
			subTree[1] = new QuadTree(environment, Rect(rect.x + rect.w / 2, rect.y, rect.w / 2, rect.h / 2));
			subTree[2] = new QuadTree(environment, Rect(rect.x, rect.y + rect.h / 2, rect.w / 2, rect.h / 2));
			subTree[3] = new QuadTree(environment, Rect(rect.w / 2 + rect.x, rect.y + rect.h / 2, rect.w / 2, rect.h / 2));
			secretSubTree = subTree;
		}
	}

	int currentEnd = start;
	for (int i = 0; i < 4; ++i) {
		subTree[i]->Build(this, currentEnd, quads, stats);
	}
}

void QuadTree::DestroySubTrees() {
	if (subTree != nullptr) {
		subTree = nullptr;
	}
}

bool QuadTree::operator==(const QuadTree &other) const {
	return end == other.end;
}

bool QuadTree::operator<(const QuadTree& other) const {
	return end < other.end;
}

bool QuadTree::operator>(const QuadTree& other) const {
	return start > other.start;
}
