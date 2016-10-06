#include "Math.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

float Math::InterpolationQuad4f(const float dy, const float dz,
								const float x1, const float x2,
								const float x3, const float x4) {
	assert(dy >= 0 && dy <= 1);
	assert(dz >= 0 && dz <= 1);
	const float idy = 1 - dy;
	const float idz = 1 - dz;
	return idy * dz * x1 + dy * dz * x2 + dy * idz * x3 + idy * idz * x4;
}
