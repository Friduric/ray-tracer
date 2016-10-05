#include "Sphere.h"

using namespace glm;

Sphere::Sphere(vec3 _center, vec3 _color, float _radius) :
	center(_center), color(_color), radius(_radius) {}

bool Sphere::RayIntersection(const Ray & ray, vec3 & intersectionPoint) {
	// We assume that the ray does not start inside the sphere.
	const float bcheck = dot(ray.dir, center - ray.from);
	if (bcheck < -FLT_EPSILON) {
		return false; // The sphere is "behind the ray".
	}

	// See http://staffwww.itn.liu.se/~mardi/WebPages/Courses/TNCG15/TheCode.pdf.
	const vec3 oc = ray.from - center;
	const float b = dot(ray.dir, oc);
	const float c = dot(oc, oc) - radius * radius;
	float k = b * b - c;

	// Change FLT_EPSILON to -FLT_EPSILON if we want border cases.
	if (k < FLT_EPSILON) { return false; } // No intersection.
	k = sqrt(k);
	float d1 = -b + k;
	float d2 = -b - k;
	if (d1 < 0) { d1 = d2; }
	if (d1 < 0) { return false; }

	// Store closest intersection point.
	float t = min(d1, d2); // Second intersection point would be max(d1, d2) if d1 != d2.
	intersectionPoint = ray.from + ray.dir * t;
	return true;
}
