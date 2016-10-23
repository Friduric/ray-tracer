#pragma once

#include <glm.hpp>

namespace Utility {
	namespace Rendering {
		float CalculateSchlicksApproximation(const glm::vec3 & incomingDirection, const glm::vec3 & normal, float n1 = 1.0f, float n2 = 1.0f);
	}
}