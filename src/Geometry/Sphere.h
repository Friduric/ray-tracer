#pragma once

#include "glm.hpp"

#include "Ray.h"

class Sphere {
public:
	glm::vec3 color;
	glm::vec3 center;
	float radius;
	Sphere(glm::vec3 center = glm::vec3(), glm::vec3 color = glm::vec3(), float r = 1.0);

	/// <summary> 
	/// Computes the ray intersection point.
	/// Returns true if there was an intersection.
	/// </summary>
	/// <param name='ray'> The ray. </param>
	/// <param name='intersectionPoint'> The overwritten output intersection point. </param>
	bool RayIntersection(const Ray& ray, glm::vec3& intersectionPoint);
};