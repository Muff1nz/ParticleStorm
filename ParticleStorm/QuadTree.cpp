#include "QuadTree.h"
#include <iostream>

QuadTree::QuadTree(Environment* environment, Rect rect_) : radiusSquared(pow(environment->circleRadius, 2)) {
	this->environment = environment;
	const int r = environment->circleRadius * 1.5f;
	rect = rect_;
	paddedRect = Rect(rect_.x - r, rect_.y - r, rect_.w + r, rect_.h +r);
}

QuadTree::~QuadTree() {
	if (subTree != nullptr) {
		for (int i = 0; i < 4; ++i) {
			delete subTree[i];
		}
	}
	delete subTree;
};

bool QuadTree::ParticleBoxCollision(const glm::vec2& circleCenter, const Rect& rect) const {
	const auto radius = environment->circleRadius;
	glm::vec2 circleDistance{};

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


void QuadTree::Build(QuadTree* parent, int& current, Stats& stats) {
	const auto particles = environment->circlePos;
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
						stats.swapCount++;
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
		end = environment->circleCount;
	}

	if (end - start >= maxParticles) {
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
			subTree[i]->Build(this, currentEnd, stats);
		}
	} else {
		if (subTree != nullptr) {
			subTree = nullptr;
		}
	}

	if (subTree == nullptr) {
		stats.leafQuadCount++;
		stats.overflowCount += overflow.size();
	}
}
