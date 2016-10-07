#include "Sphere.h"

using namespace glm;

Sphere::Sphere(vec3 _center, float _radius) :
	Primitive(), center(_center), radius(_radius) {}

glm::vec3 Sphere::GetNormal(const glm::vec3 & position) const { return glm::normalize(position - center); }

glm::vec3 Sphere::GetCenter() const {
	return center;
}

bool Sphere::RayIntersection(const Ray & ray, float & intersectionDistance) const {
	// We assume that the ray does not start inside the sphere.
	const float bcheck = dot(ray.dir, center - ray.from);
	if (bcheck < -FLT_EPSILON) {
		return false; // The sphere is "behind" the ray.
	}

	// See http://staffwww.itn.liu.se/~mardi/WebPages/Courses/TNCG15/TheCode.pdf.
	const vec3 oc = ray.from - center;
	const float b = dot(ray.dir, oc);
	const float c = dot(oc, oc) - radius * radius;
	float k = b * b - c;

	// Change FLT_EPSILON to -FLT_EPSILON if we want border cases.
	if (k < FLT_EPSILON) { return false; } // No intersection.
	k = sqrt(k);
	k -= b;
	if (k < FLT_EPSILON) { return false; }
	intersectionDistance = k;
	return true;
}
