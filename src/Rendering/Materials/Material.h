#pragma once

#include <glm.hpp>
#include "../../Geometry/Ray.h"

class Material {
public:
	float refractiveIndex, reflectivity, transparency, emissivity;

	bool IsEmissive() {return glm::abs<float>(emissivity) > FLT_EPSILON; };
	bool IsTransparent() { return transparency > FLT_EPSILON; }
	glm::vec3 GetEmissionColor() { return emissivity * GetSurfaceColor(); }
	virtual glm::vec3 GetSurfaceColor() const = 0;

	/// <summary> 
	/// Calculates the diffuse lighting given by this material. 
	/// In other words calculates the outgoing radiance. 
	/// </summary>
	/// <param name='inDirection'> The incoming direction of the light.</param>
	/// <param name='outDirection'> The outgoing direction of the light.</param>
	/// <param name='normal'> The normal of the surface.</param>
	/// <param name='incomingRadiance'> The radiance of the incoming light.</param>
	/// <param name='surfaceColor'> The color of the surface.</param>
	/// <returns> The ratio of reflected radiance exiting along the outgoing ray direction. </returns>
	virtual glm::vec3 CalculateDiffuseLighting(const glm::vec3 & inDirection, const glm::vec3 & outDirection,
											   const glm::vec3 & normal, const glm::vec3 & incomingRadiance) const = 0;
protected:
	Material(float _emissivity = 0.0f, float _reflectivity = 0.98f,
			 float _transparency = 0.0f, float _refractiveIndex = 1.0f) :
		refractiveIndex(_refractiveIndex), transparency(_transparency),
		emissivity(_emissivity), reflectivity(_reflectivity) {}
};
