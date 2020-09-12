#include "CollisionChecker.h"
#include <detail/func_geometric.inl>


CollisionChecker::CollisionChecker() {
}


CollisionChecker::CollisionChecker(float radius) {
	this->radius = radius;
	radiusSquared = radius * radius;
	doubleRadius = radius * 2;
}

CollisionChecker::~CollisionChecker() = default;

bool CollisionChecker::CircleCircleCollision(const glm::vec2 &centerOne, const glm::vec2 &centerTwo) const {
	return distance(centerOne, centerTwo) < doubleRadius;
}

bool CollisionChecker::CircleCircleCollision(const glm::vec2 &centerOne, const glm::vec2 &centerTwo, float &dist) const {
	dist = distance(centerOne, centerTwo);
	return dist < doubleRadius;
}

bool CollisionChecker::CircleRectCollision(const glm::vec2 &center, const Rect &rect) const {
	glm::vec2 circleDistance;

	circleDistance.x = abs(center.x - rect.centerX);
	circleDistance.y = abs(center.y - rect.centerY);

	if (circleDistance.x > rect.halfW + radius)  return false;
	if (circleDistance.y > rect.halfH + radius)  return false;

	if (circleDistance.x <= rect.halfW)  return true;
	if (circleDistance.y <= rect.halfH)  return true;

	const auto cornerDistanceSq =
		pow(circleDistance.x - rect.halfW, 2) +
		pow(circleDistance.y - rect.halfH, 2);

	return cornerDistanceSq <= radiusSquared;
}

bool CollisionChecker::PointInsideRect(const glm::vec2& point, const Rect& rect) {

	if (point.x > rect.x && point.x < (rect.w + rect.x) 
		&& point.y > rect.y && point.y < (rect.h + rect.y))
		return true;

	return false;
}
