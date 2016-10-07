#include "OrenNayarMaterial.h"

OrenNayarMaterial::OrenNayarMaterial(glm::vec3 surfaceColor, glm::vec3 emissionColor) {}

bool OrenNayarMaterial::IsEmissive() const {
	return false;
}

glm::vec3 OrenNayarMaterial::GetSurfaceColor() const { return glm::vec3(); }

glm::vec3 OrenNayarMaterial::GetEmissionColor() const { return glm::vec3(); }

float OrenNayarMaterial::AzimuthDistributionFunction() { return 0.0f; }

float OrenNayarMaterial::InclinationDistributionFunction() { return 0.0f; }

glm::vec3 OrenNayarMaterial::CalculateBRDF(const glm::vec3 & inDirection,
										   const glm::vec3 & outDirection,
										   const glm::vec3 & normal,
										   const glm::vec3 & incomingRadiance) const {
	return glm::vec3();
}
