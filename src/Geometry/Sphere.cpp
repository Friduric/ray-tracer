#include "Sphere.h"

using namespace glm;

Sphere::Sphere(vec3 _center, vec3 _color, float _radius) :
	center(_center), color(_color), radius(_radius) {}

int Sphere::RayIntersection(const Ray & ray, vec3 & intersectionPoint1, vec3 & intersectionPoint2) {
	return 0;
}
