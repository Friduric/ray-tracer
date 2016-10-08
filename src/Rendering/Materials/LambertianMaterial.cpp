#include "LambertianMaterial.h"

LambertianMaterial::LambertianMaterial(glm::vec3 color, glm::vec3 emission) :
	surfaceColor(color), emissionColor(emission) {}

bool LambertianMaterial::IsEmissive() const {
	return (emissionColor.r + emissionColor.g + emissionColor.b) > 3.0f * FLT_EPSILON;
}

glm::vec3 LambertianMaterial::GetSurfaceColor() const { return surfaceColor; }

glm::vec3 LambertianMaterial::GetEmissionColor() const { return emissionColor; }

glm::vec3 LambertianMaterial::CalculateBRDF(const glm::vec3 & inDirection,
											const glm::vec3 & outDirection,
											const glm::vec3 & normal,
											const glm::vec3 & incomingRadiance) const {
	float d = glm::max(0.0f, reflectance * glm::dot(inDirection, normal));
	float r = incomingRadiance.r * surfaceColor.r;
	float g = incomingRadiance.g * surfaceColor.g;
	float b = incomingRadiance.b * surfaceColor.b;
	return d * glm::vec3(r, g, b);
}

