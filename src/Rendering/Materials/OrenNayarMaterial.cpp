#include "OrenNayarMaterial.h"

#include "../../../includes/glm/gtc/constants.hpp"
#include <algorithm>
#include <iostream>


OrenNayarMaterial::OrenNayarMaterial(glm::vec3 color, float _roughness, float _emissivity,
									 float _reflectivity, float _transparency,
									 float _refractiveIndex, float _specularity, float _specularExponent) :
	surfaceColor(color), roughness(_roughness),
	Material(_emissivity, _reflectivity, _transparency, _refractiveIndex, _specularity, _specularExponent) {}

glm::vec3 OrenNayarMaterial::GetSurfaceColor() const { return surfaceColor; }

glm::vec3 OrenNayarMaterial::CalculateDiffuseLighting(const glm::vec3 & inDirection,
													  const glm::vec3 & outDirection,
													  const glm::vec3 & normal,
													  const glm::vec3 & incomingRadiance) const {
	assert(glm::length(inDirection) < 1.0f + 10.0f * FLT_EPSILON && glm::length(inDirection) > 1.0f - 10.0f * FLT_EPSILON);
	assert(glm::length(outDirection) < 1.0f + 10.0f * FLT_EPSILON && glm::length(outDirection) > 1.0f - 10.0f * FLT_EPSILON);
	// See https://en.wikipedia.org/wiki/Oren-Nayar_reflectance_model for more information.

	const float indot = glm::dot(-inDirection, normal);
	if (indot < FLT_EPSILON) {
		return glm::vec3(0);
	}

	const float sigma2 = roughness * roughness;

	const float A = 1 - 0.5f * sigma2 / (sigma2 + 0.57f);
	const float B = 0.45f * sigma2 / (sigma2 + 0.09f);

	const float alphaInclination = acos(indot);
	const float betaInclination = acos(glm::dot(outDirection, normal));

	const float alpha = std::max(alphaInclination, betaInclination);
	const float beta = std::min(alphaInclination, betaInclination);

	const float C = glm::max<float>(0.0f, glm::dot(-inDirection, outDirection));
	const float D = (C > 0.0f) ? B * C * sin(alpha) * tan(beta) : 0.0f;

	return glm::max(0.0f, indot) * glm::min<float>((A + D), 1.0f) * (incomingRadiance * surfaceColor);
}
