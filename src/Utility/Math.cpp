#include "Math.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

float Math::BilinearInterpolation(const float dy, const float dz,
								  const float x1, const float x2,
								  const float x3, const float x4) {
	assert(dy >= 0 && dy <= 1 + FLT_EPSILON);
	assert(dz >= 0 && dz <= 1 + FLT_EPSILON);
	const float idy = 1 - dy;
	const float idz = 1 - dz;
	float a1 = idy * dz; // lower right area.
	float a2 = dy * dz; // lower left area.
	float a3 = dy * idz; // upper left area.
	float a4 = idy * idz; // upper right area.
	assert(abs(a1 + a2 + a3 + a4 - 1.0f) < FLT_EPSILON * 10.0f);
	return a3 * x1 + a4 * x2 + a1 * x3 + a2 * x4;
}

glm::vec3 Math::NonParallellVector(const glm::vec3 & v) {
	if (abs(v.x) < FLT_EPSILON) {
		return glm::vec3(1, 0, 0);
	}
	else if (abs(v.y) < FLT_EPSILON) {
		return glm::vec3(0, 1, 0);
	}
	else if (abs(v.z) < FLT_EPSILON) {
		return glm::vec3(0, 0, 1);
	}
	else {
		// None of v-parameters are zero. 
		return glm::vec3(-v.y, v.x, 0);
	}
}

Math::NormalDistributionGenerator::NormalDistributionGenerator(float _min, float _max) :
	min(_min), max(_max), distribution(0.5f * (_min + _max), (1.0f / 6.0f) * (_max - _min)) { }

float Math::NormalDistributionGenerator::GetRandomFloat() {
	float v = distribution(generator);
	if (v < min - FLT_EPSILON) { return min; }
	if (v > max + FLT_EPSILON) { return max; }
	return v;
}
