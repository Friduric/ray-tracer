#pragma once

#include "glm.hpp"
#include "Primitive.h"
#include "Ray.h"

class Sphere : public Primitive {
public:
	glm::vec3 center;
	float radius;
	Sphere(glm::vec3 center = glm::vec3(), float r = 1.0);

	glm::vec3 GetNormal(const glm::vec3 & position) const override;

	/// <summary> 
	/// Computes the ray intersection point.
	/// Returns true if there was an intersection.
	/// </summary>
	/// <param name='ray'> The ray. </param>
	/// <param name='intersectionPoint'> The overwritten output intersection point. </param>
	bool RayIntersection(const Ray& ray, glm::vec3& intersectionPoint) const override;
};