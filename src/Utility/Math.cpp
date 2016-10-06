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
	float alpha1 = 1.0f - sqrt(idy * idy + dz * dz); // Distance to "lower right" point.
	float alpha2 = 1.0f - sqrt(dy * dy + dz * dz); // Distance to "lower left" point.
	float alpha3 = 1.0f - sqrt(dy * dy + idz * idz); // Distance to "upper left" point.
	float alpha4 = 1.0f - sqrt(idy * idy + idz * idz); // Distance to "upper right" point.
	float alphaSum = alpha1 + alpha2 + alpha3 + alpha4;
	alpha1 /= alphaSum;
	alpha2 /= alphaSum;
	alpha3 /= alphaSum;
	alpha4 /= alphaSum;
	assert(alpha1 >= 0 && alpha1 <= 1 + FLT_EPSILON);
	assert(alpha2 >= 0 && alpha2 <= 1 + FLT_EPSILON);
	assert(alpha3 >= 0 && alpha3 <= 1 + FLT_EPSILON);
	assert(alpha4 >= 0 && alpha4 <= 1 + FLT_EPSILON);
	assert(alpha1 + alpha2 + alpha3 + alpha4 >= 1 - FLT_EPSILON && alpha1 + alpha2 + alpha3 + alpha4 <= 1 + FLT_EPSILON);
	return alpha1 * x1 + alpha2 * x2 + alpha3 * x3 + alpha4 * x4;
}
