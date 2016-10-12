#pragma once

#include <glm.hpp>

/// <summary> Describes a 3D ray. </summary>
class Ray {
public:
	// Ray is parametrized as X = F + t * D (where F = from and D = direction).
	glm::vec3 direction, from;

	/* Extra data needed for fast AAABB intersection testing. */
	float idirx, idiry, idirz; // TODO: Change name of these to something more intuitive.
	float cxy, cxz, cyx, cyz, czx, czy; // TODO: Change name of these to something more intuitive.
	float ibyj, jbyi, jbyk, kbyj, ibyk, kbyi; // TODO: Change name of these to something more intuitive.

	Ray(glm::vec3 from, glm::vec3 direction);
	Ray();

	/// <summary> 
	/// Updates ray AABB intersection testing data. This should be called whenever the ray is altered.
	/// </summary>
	void Update();
};