#include "Sphere.h"

#include <iostream>

using namespace glm;

Sphere::Sphere(vec3 _center, float _radius) :
	Primitive(), center(_center), radius(_radius) {}

glm::vec3 Sphere::GetNormal(const glm::vec3 & position) const { return glm::normalize(position - center); }

glm::vec3 Sphere::GetCenter() const {
	return center;
}

bool Sphere::RayIntersection(const Ray & ray, float & intersectionDistance) const {
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
	d = sqrt(d);
	if (b > d) {
		return false;
	}
	intersectionDistance = -b - d;
	assert(intersectionDistance > FLT_EPSILON);
	return true;
}
