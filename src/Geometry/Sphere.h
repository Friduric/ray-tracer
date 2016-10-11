#pragma once

#include "glm.hpp"
#include "Primitive.h"
#include "Ray.h"

/// <summary> Describes a 3D sphere. </summary>
class Sphere : public Primitive {
public:
	glm::vec3 center;
	float radius;

	Sphere(glm::vec3 center = glm::vec3(), float r = 1.0);

	glm::vec3 GetNormal(const glm::vec3 & position) const override;
	glm::vec3 GetCenter() const override;
	glm::vec3 GetRandomPositionOnSurface() const override;
	const AABB & GetAxisAlignedBoundingBox() const override;

	/// <summary> 
	/// Computes the ray intersection point between a ray and this sphere.
	/// Returns true if there was an intersection.
	/// The ray is assumed to start outside of the sphere.
	/// </summary>
	/// <param name='ray'> The ray for which we compute sphere intersection. </param>
	/// <param name='intersectionPoint'> 
	/// OUT: The distance to the intersection point (if there is an intersection). 
	/// </param>
	bool RayIntersection(const Ray& ray, float & intersectionDistance) const override;
private:
	AABB axisAlignedBoundingBox;
};