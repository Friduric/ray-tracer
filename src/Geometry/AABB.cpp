#include "AABB.h"

#include <iostream>

AABB::AABB() : AABB(-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f) {}

AABB::AABB(const glm::vec3 _min, const glm::vec3 _max) : minimum(_min), maximum(_max) {}

AABB::AABB(const float minX, const float minY, const float minZ,
		   const float maxX, const float maxY, const float maxZ) :
	AABB(glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ)) {}

bool AABB::IsPointInsideAABB(const glm::vec3 & p) const {
	if (p.x < minimum.x || p.y < minimum.y || p.z < minimum.z) {
		return false;
	}
	if (p.x > maximum.x || p.y > maximum.y || p.z > maximum.z) {
		return false;
	}
	return true;
}

glm::vec3 AABB::GetCenter() const { return 0.5f * (minimum + maximum); }

bool AABB::RayIntersection(const Ray & ray) {
	// Using the fast ray AABB overlap test using ray slopes as described by M. Eisemann et al.
	// http://www.cg.cs.tu-bs.de/media/publications/fast-rayaxis-aligned-bounding-box-overlap-tests-using-ray-slopes.pdf
	std::cerr << "AABB ray intersection has not been implemented yet." << std::endl;
	return false;
}
