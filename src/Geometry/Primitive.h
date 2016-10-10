#pragma once

#include "glm.hpp"
#include "../Rendering/Materials/Material.h"
#include "Ray.h"

class Primitive {
public:
	virtual glm::vec3 GetNormal(const glm::vec3 & position) const = 0;
	virtual glm::vec3 GetCenter() const = 0;
	virtual glm::vec3 GetRandomPositionOnSurface() const = 0;
	virtual glm::vec3 GetAxisAlignedBoundingBox(float & outMinX, float & outMaxX,
												float & outMinY, float & outMaxY,
												float & outMinZ, float & outMaxZ) const = 0;

	/// <summary> Calculates the outgoing ray position. </summary>
	virtual glm::vec3 ComputeOutgoingPosition(const glm::vec3 & incomingPosition,
											  const glm::vec3 & incomingDirection,
											  const float refractionIndexOfIncomingMedium,
											  const Material & material) const = 0;
	/// <summary> 
	/// Computes the ray intersection point.
	/// Returns true if there is an intersection.
	/// </summary>
	/// <param name='ray'> The ray for which we compute intersection. </param>
	/// <param name='intersectionPoint'> 
	/// The distance to the intersection point (if there is an intersection). 
	/// </param>
	virtual bool RayIntersection(const Ray& ray, float & intersectionDistance) const = 0;
};