#include "Triangle.h"
#include "../../includes/glm/gtx/norm.hpp"
#include <iostream>

using namespace glm;

Triangle::Triangle(vec3 v1, vec3 v2, vec3 v3, vec3 _normal) : vertices{ v1,v2,v3 }, normal(_normal) { }

Triangle::Triangle(vec3 _vertices[3], vec3 _normal) : normal(_normal) {
	for (unsigned int i = 0; i < 3; ++i) { vertices[i] = _vertices[i]; }
}

vec3 Triangle::GetNormal(const vec3 & position) const { return normal; }

glm::vec3 Triangle::GetCenter() const {
	return (vertices[0] + vertices[1] + vertices[2]) / 3.0f;
}

glm::vec3 Triangle::GetRandomPositionOnSurface() const
{
	return glm::vec3();
}

glm::vec3 Triangle::GetAxisAlignedBoundingBox(float & minX, float & maxX, float & minY, float & maxY, float & minZ, float & maxZ) const
{
	return glm::vec3();
}

// Implementation using the M�ller-Trumbore (MT) ray intersection algorithm.
bool Triangle::RayIntersection(const Ray & ray, float & intersectionDistance) const {
	if (dot(ray.dir, normal) > -FLT_EPSILON) {
		return false; // The ray direction and normal are in the same direction (behind).
	}

	// Calculate intersection using barycentric coordinates. This gives a equation system
	// which we can solve using Cramer's rule.
	const vec3 E1 = vertices[1] - vertices[0];
	const vec3 E2 = vertices[2] - vertices[0];
	const vec3 P = cross(ray.dir, E2);
	const vec3 T = ray.from - vertices[0];

	const float inv_den = 1.0f / dot(E1, P);
	float u = inv_den * dot(T, P);
	if (u < -FLT_EPSILON || u > 1.0f + FLT_EPSILON) {
		return false; // Didn't hit.
	}

	const vec3 Q = cross(T, E1);
	const float v = inv_den * dot(ray.dir, Q);
	if (v < -FLT_EPSILON || u + v > 1.0f + FLT_EPSILON) {
		return false; // Didn't hit.
	}

	intersectionDistance = inv_den * dot(E2, Q);
	if (intersectionDistance < FLT_EPSILON) {
		return false;
	}

	return true;
}
