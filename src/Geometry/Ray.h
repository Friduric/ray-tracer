#pragma once

#include <glm.hpp>

#define __AABB_TESTING false

/// <summary> 
/// Describes a 3D ray. 
/// Ray is parametrized as X = F + t * D (where F = from and D = direction).
/// </summary>
class Ray {
public:
	glm::vec3 direction, from;
	Ray(glm::vec3 from, glm::vec3 direction);
	Ray();
#if __AABB_TESTING
	/// <summary> 
	/// Updates ray AABB intersection testing data. This should be called whenever the ray is altered.
	/// </summary>
	void Update();

	// ------------------------------------------------------
	// Extra data needed for fast AAABB intersection testing.
	// ------------------------------------------------------
	float idirx, idiry, idirz; // TODO: Change name of these to something more intuitive.
	float cxy, cxz, cyx, cyz, czx, czy; // TODO: Change name of these to something more intuitive.
	float ibyj, jbyi, jbyk, kbyj, ibyk, kbyi; // TODO: Change name of these to something more intuitive.
	// ------------------------------------------------------
#endif
};