#include "LambertianMaterial.h"

LambertianMaterial::LambertianMaterial(glm::vec3 color, glm::vec3 emission) :
	surfaceColor(color), emissionColor(emission) {}

bool LambertianMaterial::IsEmissive() const {
	return (emissionColor.r + emissionColor.g + emissionColor.b) > 3.0f * FLT_EPSILON;
}

glm::vec3 LambertianMaterial::GetSurfaceColor() const { return surfaceColor; }

glm::vec3 LambertianMaterial::GetEmissionColor() const { return emissionColor; }

float LambertianMaterial::CalculateBRDF(const glm::vec3 & inDirection,
										const glm::vec3 & outDirection,
										const glm::vec3 & normal,
										const glm::vec3 & incomingRadiance) const {
	return albedo * glm::max(0.0f, glm::dot(inDirection, normal));
}


