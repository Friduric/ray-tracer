#include "Math.h"

#include <algorithm>
#include <cassert>

float Math::InterpolationQuad4f(float dy, float dz, float x1, float x2, float x3, float x4) {
	assert(dy >= 0 && dy <= 1);
	assert(dz >= 0 && dz <= 1);
	float idy = 1 - dy;
	float idz = 1 - dz;
	float dist1 = sqrt(idy * idy + dz * dz); // Distance to "lower right" point.
	float dist2 = sqrt(dy * dy + dz * dz); // Distance to "lower left" point.
	float dist3 = sqrt(dy * dy + idz * idz); // Distance to "upper left" point.
	float dist4 = sqrt(idy * idy + idz * idz); // Distance to "upper right" point.
	assert(dist1 >= 0 && dist1 <= 1);
	assert(dist2 >= 0 && dist2 <= 1);
	assert(dist3 >= 0 && dist3 <= 1);
	assert(dist4 >= 0 && dist4 <= 1);
	return (1.0 - dist1) * x1 + (1.0 - dist2) * x2 + (1.0 - dist3) * x3 + (1.0 - dist4) * x4;
}
