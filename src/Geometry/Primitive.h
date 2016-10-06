#pragma once

#include "glm.hpp"

#include "Ray.h"

class Primitive {
public:
	glm::vec3 color;

	/// <summary> 
	/// Computes the ray intersection point.
	/// Returns true if there is an intersection.
	/// </summary>
	/// <param name='ray'> The ray. </param>
	/// <param name='intersectionPoint'> The overwritten output intersection point. </param>
	virtual bool RayIntersection(const Ray& ray, glm::vec3& intersectionPoint) const = 0;
};