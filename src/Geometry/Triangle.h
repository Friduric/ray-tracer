#pragma once

#include "glm.hpp"

#include "Ray.h"

class Triangle {
public:
	glm::vec3 color;
	glm::vec3 vertices[3];
	glm::vec3 normal;
	Triangle(glm::vec3 v1 = glm::vec3(), glm::vec3 v2 = glm::vec3(), glm::vec3 v3 = glm::vec3(),
			 glm::vec3 normal = glm::vec3(), glm::vec3 color = glm::vec3());
	Triangle(glm::vec3 vertices[3], glm::vec3 normal = glm::vec3(), glm::vec3 color = glm::vec3());

	/// <summary> 
	/// Computes the ray intersection point.
	/// Returns true if there is an intersection.
	/// </summary>
	/// <param name='ray'> The ray. </param>
	/// <param name='intersectionPoint'> The overwritten output intersection point. </param>
	bool RayIntersection(const Ray& ray, glm::vec3& intersectionPoint) const;
};