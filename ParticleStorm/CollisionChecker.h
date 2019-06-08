#pragma once
#include <vec2.hpp>
#include "Rect.h"

class CollisionChecker {
public:
	CollisionChecker();
	CollisionChecker(float radius);
	~CollisionChecker();

	bool CircleCircleCollision(const glm::vec2 &centerOne, const glm::vec2 &centerTwo) const;
	bool CircleCircleCollision(const glm::vec2 &centerOne, const glm::vec2 &centerTwo, float &dist) const;
	bool CircleRectCollision(const glm::vec2& center, const Rect& rect) const;

private:
	float radius;
	float radiusSquared;
	float doubleRadius;
};

