#include "LambertianMaterial.h"

glm::vec3 LambertianMaterial::CalculateBRDF(glm::vec3 & inDirection, glm::vec3 & outDirection,
											glm::vec3 & normal, glm::vec3 & incomingIntensity,
											glm::vec3 & surfaceColor) {
	float d = glm::max(0.0f, glm::dot(inDirection, normal));
	float r = incomingIntensity.x * surfaceColor.x;
	float g = incomingIntensity.y * surfaceColor.y;
	float b = incomingIntensity.z * surfaceColor.z;
	return d * glm::vec3(r, g, b);
}

