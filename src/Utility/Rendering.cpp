#include "Rendering.h"

float Utility::Rendering::CalculateSchlicksApproximation(const glm::vec3 & incomingDirection, const glm::vec3 & normal, float n1, float n2) {
	// See https://en.wikipedia.org/wiki/Schlick%27s_approximation for more information.
	float R0 = glm::pow((n1 - n2) / (n1 + n2), 2.0f);
	float alpha = glm::dot(normal, incomingDirection); // Should be the angle of the halfway vector ("from origin").
	return R0 + (1 - R0) * glm::pow((1 - alpha), 5.0f);
}
