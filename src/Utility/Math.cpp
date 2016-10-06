#include "Math.h"

#include <algorithm>
#include <cassert>

float Math::InterpolationQuad4f(const float dy, const float dz,
								const float x1, const float x2,
								const float x3, const float x4) {
	assert(dy >= 0 && dy <= 1);
	assert(dz >= 0 && dz <= 1);
	const float idy = 1 - dy;
	const float idz = 1 - dz;
	float dist1 = sqrt(idy * idy + dz * dz); // Distance to "lower right" point.
	float dist2 = sqrt(dy * dy + dz * dz); // Distance to "lower left" point.
	float dist3 = sqrt(dy * dy + idz * idz); // Distance to "upper left" point.
	float dist4 = sqrt(idy * idy + idz * idz); // Distance to "upper right" point.
	float distsum = dist1 + dist2 + dist3 + dist4;
	dist1 /= distsum;
	dist2 /= distsum;
	dist3 /= distsum;
	dist4 /= distsum;
	assert(dist1 >= 0 && dist1 <= 1 + FLT_EPSILON);
	assert(dist2 >= 0 && dist2 <= 1 + FLT_EPSILON);
	assert(dist3 >= 0 && dist3 <= 1 + FLT_EPSILON);
	assert(dist4 >= 0 && dist4 <= 1 + FLT_EPSILON);
	return (1.0f - dist1) * x1 + (1.0f - dist2) * x2 + (1.0f - dist3) * x3 + (1.0f - dist4) * x4;
}
