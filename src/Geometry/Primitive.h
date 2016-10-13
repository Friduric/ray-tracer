#pragma once

#include "glm.hpp"

#include "AABB.h"
#include "../Rendering/Materials/Material.h"
#include "Ray.h"

/// <summary> Abstract base class for geometrical primitives such as spheres and triangles </summary> 
class Primitive {
public:
	bool convex = true;
	virtual glm::vec3 GetNormal(const glm::vec3 & position) const = 0;
	virtual glm::vec3 GetCenter() const = 0;
	virtual glm::vec3 GetRandomPositionOnSurface() const = 0;
	virtual const AABB & GetAxisAlignedBoundingBox() const = 0;

	/// <summary> 
	/// Computes the ray intersection point.
	/// Returns true if there is an intersection.
	/// </summary>
	/// <param name='ray'> The ray for which we compute intersection. </param>
	/// <param name='intersectionPoint'> 
	/// The distance to the intersection point (if there is an intersection). 
	/// </param>
	virtual bool RayIntersection(const Ray& ray, float & intersectionDistance, bool cullBackFace = true) const = 0;
};