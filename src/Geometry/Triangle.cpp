#include "Triangle.h"

#include "../../includes/glm/gtx/norm.hpp"

// Default constructor.
Triangle::Triangle(glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, glm::vec3 _normal) :
	vertices{ _v1, _v2, _v3 }, normal(_normal) {

	const auto & v0 = vertices[0];
	const auto & v1 = vertices[1];
	const auto & v2 = vertices[2];

	glm::vec3 minimum;
	minimum.x = glm::min<float>(v0.x, glm::min<float>(v1.x, v2.x));
	minimum.y = glm::min<float>(v0.y, glm::min<float>(v1.y, v2.y));
	minimum.z = glm::min<float>(v0.z, glm::min<float>(v1.z, v2.z));

	glm::vec3 maximum;
	maximum.x = glm::max<float>(v0.x, glm::max<float>(v1.x, v2.x));
	maximum.y = glm::max<float>(v0.y, glm::max<float>(v1.y, v2.y));
	maximum.z = glm::max<float>(v0.z, glm::max<float>(v1.z, v2.z));

	axisAlignedBoundingBox = AABB(minimum, maximum);
}

// Shadowed.
Triangle::Triangle(glm::vec3 _vertices[3], glm::vec3 _normal) :
	Triangle(_vertices[0], _vertices[1], _vertices[2], _normal) { }

glm::vec3 Triangle::GetNormal(const glm::vec3 & position) const { return normal; }

glm::vec3 Triangle::GetCenter() const {
	return (vertices[0] + vertices[1] + vertices[2]) / 3.0f;
}

glm::vec3 Triangle::GetRandomPositionOnSurface() const {
	glm::vec3 v;
	float quadArea = glm::length(glm::cross(vertices[0] - vertices[1], vertices[0] - vertices[2]));
	float a1, a2, a3;
	do {
		float rand1 = rand() / (float)RAND_MAX;
		float rand2 = rand() / (float)RAND_MAX;
		a1 = glm::length(glm::cross(v - vertices[0], v - vertices[1]));
		a2 = glm::length(glm::cross(v - vertices[1], v - vertices[2]));
		a3 = glm::length(glm::cross(v - vertices[2], v - vertices[0]));
		v = vertices[0] + rand1 * (vertices[1] - vertices[0]) + rand2 * (vertices[2] - vertices[0]);
	} while (glm::length(a1 + a2 + a3 - quadArea) > FLT_EPSILON);
	return v;
}

const AABB & Triangle::GetAxisAlignedBoundingBox() const {
	return axisAlignedBoundingBox;
}

// Implementation using the Möller-Trumbore (MT) ray intersection algorithm.
bool Triangle::RayIntersection(const Ray & ray, float & intersectionDistance, bool cullBackFace) const {
	if (cullBackFace && glm::dot(ray.dir, normal) > -FLT_EPSILON) {
		return false; // The ray direction and normal are in the same direction (behind).
	}

	// Calculate intersection using barycentric coordinates. This gives a equation system
	// which we can solve using Cramer's rule.
	const glm::vec3 E1 = vertices[1] - vertices[0];
	const glm::vec3 E2 = vertices[2] - vertices[0];
	const glm::vec3 P = glm::cross(ray.dir, E2);
	const glm::vec3 T = ray.from - vertices[0];

	const float inv_den = 1.0f / glm::dot(E1, P);
	float u = inv_den * glm::dot(T, P);
	if (u < -FLT_EPSILON || u > 1.0f + FLT_EPSILON) {
		return false; // Didn't hit.
	}

	const glm::vec3 Q = glm::cross(T, E1);
	const float v = inv_den * glm::dot(ray.dir, Q);
	if (v < -FLT_EPSILON || u + v > 1.0f + FLT_EPSILON) {
		return false; // Didn't hit.
	}

	intersectionDistance = inv_den * glm::dot(E2, Q);
	return intersectionDistance > FLT_EPSILON;
}
