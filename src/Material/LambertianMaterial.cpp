#include "LambertianMaterial.h"

glm::vec3 LambertianMaterial::CalculateBRDF(glm::vec3 & inDirection, glm::vec3 & outDirection,
											glm::vec3 & normal, glm::vec3 & incomingIntensity,
											glm::vec3 & surfaceColor) {
	float dotProduct = glm::max(0.0f, glm::dot(inDirection, normal));
	return dotProduct * glm::vec3(incomingIntensity.x * surfaceColor.x,
								  incomingIntensity.y * surfaceColor.y,
								  incomingIntensity.z * surfaceColor.z);
}

