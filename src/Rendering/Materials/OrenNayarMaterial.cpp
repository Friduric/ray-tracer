#include "OrenNayarMaterial.h"

#include <algorithm>

OrenNayarMaterial::OrenNayarMaterial(glm::vec3 _surfaceColor, glm::vec3 _emissionColor) :
	surfaceColor(_surfaceColor), emissionColor(_emissionColor) {}

bool OrenNayarMaterial::IsEmissive() const {
	return (emissionColor.r + emissionColor.g + emissionColor.b) > 3.0f * FLT_EPSILON;
}

glm::vec3 OrenNayarMaterial::GetSurfaceColor() const { return surfaceColor; }

glm::vec3 OrenNayarMaterial::GetEmissionColor() const { return emissionColor; }

glm::vec3 OrenNayarMaterial::CalculateBRDF(const glm::vec3 & inDirection,
										   const glm::vec3 & outDirection,
										   const glm::vec3 & normal,
										   const glm::vec3 & incomingRadiance) const {
	float roughnessSquared = roughness * roughness;

	float cosin = glm::dot(normal, inDirection);
	float cosout = glm::dot(normal, outDirection);

	float alphaInclination = acos(cosout);
	float betaInclination = acos(cosin);

	float gamma = glm::dot(outDirection - normal * cosout, inDirection - normal * cosin);

	float A = 1 - 0.5f * roughnessSquared / (roughnessSquared + 0.57f);
	float B = 0.45f * roughnessSquared / (roughnessSquared + 0.09f);

	float alpha = std::max(cosin, cosout);

	float res = std::max(0.0f, cosin) * (alpha + B * std::max(0.0f, gamma) * sin(A) * tan(B));

	float r = incomingRadiance.r * surfaceColor.r;
	float g = incomingRadiance.g * surfaceColor.g;
	float b = incomingRadiance.b * surfaceColor.b;

	return res * glm::vec3(r, g, b);
}
