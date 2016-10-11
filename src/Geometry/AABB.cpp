#include "AABB.h"

AABB::AABB(const glm::vec3 _min, const glm::vec3 _max) : minimum(_min), maximum(_max) {}

bool AABB::IsPointInsideAABB(const glm::vec3 & p) const {
	if (p.x < minimum.x || p.y < minimum.y || p.z < minimum.z) {
		return false;
	}
	if (p.x > maximum.x || p.y > maximum.y || p.z > maximum.z) {
		return false;
	}
	return true;
}
