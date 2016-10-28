#pragma once

#include "../../../includes/glm/gtc/constants.hpp"
#include <glm.hpp>
#include "../../Geometry/Ray.h"

#include <iostream> // TODO: Remove this.

class Material {
public:
	float refractiveIndex, reflectivity, transparency, emissivity, specularity, specularExponent;

	bool IsEmissive() const { return emissivity > FLT_EPSILON; };
	bool IsTransparent() const { return transparency > FLT_EPSILON; }
	bool IsSpecular() const { return specularity > FLT_EPSILON; }
	bool IsReflective() const { return reflectivity > FLT_EPSILON; }
	virtual glm::vec3 GetEmissionColor() const { return emissivity * GetSurfaceColor(); }
	virtual glm::vec3 GetSurfaceColor() const = 0;

	/// <summary> 
	/// Calculates the diffuse lighting given by this material. 
	/// In other words calculates the outgoing radiance given the incoming radiance. 
	/// </summary>
	/// <param name='inDirection'> The incoming direction of the light.</param>
	/// <param name='outDirection'> The outgoing direction of the light.</param>
	/// <param name='normal'> The normal of the surface.</param>
	/// <param name='incomingRadiance'> The radiance of the incoming light.</param>
	/// <param name='surfaceColor'> The color of the surface.</param>
	/// <returns> The ratio of reflected radiance exiting along the outgoing ray direction. </returns>
	virtual glm::vec3 CalculateDiffuseLighting(const glm::vec3 & inDirection, const glm::vec3 & outDirection,
											   const glm::vec3 & normal, const glm::vec3 & incomingRadiance) const = 0;
	virtual glm::vec3 CalculateSpecularLighting(const glm::vec3 & inDirection, const glm::vec3 & outDirection,
												const glm::vec3 & normal, const glm::vec3 & incomingRadiance) const {
		const glm::vec3 lightReflection = glm::reflect(inDirection, normal);
		float sqr = glm::pow<float>(glm::dot(lightReflection, outDirection), specularExponent); // Phong model.
		return  glm::max(0.0f, sqr) * incomingRadiance;
	}

protected:
	Material(float _emissivity = 0.0f, float _reflectivity = 0.98f,
			 float _transparency = 0.0f, float _refractiveIndex = 1.0f,
			 float _specularity = 0.0f, float _specularityExponent = 75.0f) :
		refractiveIndex(_refractiveIndex), transparency(_transparency),
		emissivity(_emissivity), reflectivity(_reflectivity), specularity(_specularity), specularExponent(_specularityExponent) {}
};
