#include "LambertianMaterial.h"

glm::vec3 LambertianMaterial::CalculateBRDF(const glm::vec3 & inDirection, const glm::vec3 & outDirection,
											const glm::vec3 & normal, const glm::vec3 & incomingIntensity,
											const glm::vec3 & surfaceColor) const {
	float d = glm::max(0.0f, glm::dot(inDirection, normal));
	float r = incomingIntensity.r * surfaceColor.r;
	float g = incomingIntensity.g * surfaceColor.g;
	float b = incomingIntensity.b * surfaceColor.b;
	return d * glm::vec3(r, g, b);
}

