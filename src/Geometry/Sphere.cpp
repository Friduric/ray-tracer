#include "Sphere.h"

#include <iostream>
#include "../Utility/Math.h"

using namespace glm;

Sphere::Sphere(vec3 _center, float _radius) :
	Primitive(), center(_center), radius(_radius) {}

glm::vec3 Sphere::GetNormal(const glm::vec3 & position) const { return glm::normalize(position - center); }

glm::vec3 Sphere::GetCenter() const {
	return center;
}

glm::vec3 Sphere::GetRandomPositionOnSurface() const
{
	int direction = (rand() & 1) == 0 ? -1 : 1;
	return center + radius * Math::CosineWeightedHemisphereSampleDirection(glm::vec3(0, 0, direction));
}

bool Sphere::RayIntersection(const Ray & ray, float & intersectionDistance) const {
	if (dot(ray.dir, GetNormal(ray.from)) > -FLT_EPSILON) {
		return false; // The ray direction and normal are in the same direction (behind).
	}
	const vec3 m = ray.from - center;
	float b = glm::dot(m, ray.dir);
	float c = glm::dot(m, m) - radius * radius;
	if (c > FLT_EPSILON && b > FLT_EPSILON) {
		return false;
	}
	float d = b * b - c;
	if (d < FLT_EPSILON) {
		return false;
	}
	intersectionDistance = -b - sqrt(d);
	if (intersectionDistance < FLT_EPSILON) {
		return false;
	}
	assert(intersectionDistance > FLT_EPSILON);
	return true;
}
