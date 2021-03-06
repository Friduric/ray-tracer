#include "Math.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "../../includes/glm/gtx/norm.hpp"
#include "../../includes/glm/gtx/rotate_vector.hpp"

using namespace std;

float Utility::Math::BilinearInterpolation(const float dy, const float dz, const float x1, const float x2, const float x3, const float x4) {
	assert(dy >= -FLT_EPSILON && dy <= 1 + FLT_EPSILON);
	assert(dz >= -FLT_EPSILON && dz <= 1 + FLT_EPSILON);
	const float idy = 1 - dy;
	const float idz = 1 - dz;
	float a1 = idy * dz; // lower right area.
	float a2 = dy * dz; // lower left area.
	float a3 = dy * idz; // upper left area.
	float a4 = idy * idz; // upper right area.
	assert(abs(a1 + a2 + a3 + a4 - 1.0f) < FLT_EPSILON * 10.0f);
	return a3 * x1 + a4 * x2 + a1 * x3 + a2 * x4;
}

glm::vec3 Utility::Math::NonParallellVector(const glm::vec3 & v) {
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
		return glm::vec3(-v.y, v.z, -v.x);
	}
}

glm::vec3 Utility::Math::RandomHemishpereSampleDirection(const glm::vec3 & n) {
	// Samples uniform angles.
	float incl = (rand() / static_cast<float>(RAND_MAX)) * glm::half_pi<float>();
	float azim = (rand() / static_cast<float>(RAND_MAX)) * glm::two_pi<float>();
	glm::vec3 nonParallellVector = Math::NonParallellVector(n);
	assert(glm::length(glm::cross(nonParallellVector, n)) > FLT_EPSILON);
	glm::vec3 rotationVector = glm::cross(nonParallellVector, n);
	glm::vec3 inclVector = rotate(n, incl, rotationVector);
	return glm::normalize(rotate(inclVector, azim, n));
}

glm::vec3 Utility::Math::CosineWeightedHemisphereSampleDirection(const glm::vec3 & n) {
	// See https://pathtracing.wordpress.com/2011/03/03/cosine-weighted-hemisphere/.
	// Samples cosine weighted positions.
	float r1 = rand() / static_cast<float>(RAND_MAX);
	float r2 = rand() / static_cast<float>(RAND_MAX);

	float theta = acos(sqrt(1.0f - r1));
	float phi = 2.0f * glm::pi<float>() * r2;

	float xs = sinf(theta) * cosf(phi);
	float ys = cosf(theta);
	float zs = sinf(theta) * sinf(phi);

	glm::vec3 y(n.x, n.y, n.z);
	glm::vec3 h = y;
	if (abs(h.x) <= abs(h.y) && abs(h.x) <= abs(h.z)) {
		h.x = 1.0;
	}
	else if (abs(h.y) <= abs(h.x) && abs(h.y) <= abs(h.z)) {
		h.y = 1.0;
	}
	else {
		h.z = 1.0;
	}

	glm::vec3 x = glm::normalize(glm::cross(h, y));
	glm::vec3 z = glm::normalize(glm::cross(x, y));
	return glm::normalize(xs * x + ys * y + zs * z);
}

Utility::Math::NormalDistributionGenerator::NormalDistributionGenerator(float _min, float _max) :
	min(_min), max(_max), distribution(0.5f * (_min + _max), (1.0f / 6.0f) * (_max - _min)) { }

float Utility::Math::NormalDistributionGenerator::GetRandomFloat() {
	float v = distribution(generator);
	if (v < min - FLT_EPSILON) { return min; }
	if (v > max + FLT_EPSILON) { return max; }
	return v;
}
