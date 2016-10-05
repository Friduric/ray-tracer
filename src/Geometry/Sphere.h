#pragma once

#include "glm.hpp"
#include "Ray.h"

class Sphere {
public:
	glm::vec3 color;
	glm::vec3 center;
	double radius;
	Sphere(glm::vec3 center = glm::vec3(), glm::vec3 color = glm::vec3(), double r = 1.0);

	// Returns the number of intersection points given by a ray.
	// Overwrites the two intersection point vertices with the intersection (if there was an intersection).
	int RayIntersection(const Ray& ray, glm::vec3& intersectionPoint1, glm::vec3& intersectionPoint2);
};