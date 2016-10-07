#pragma once

#include <random>
#include <glm.hpp>
#include "../../includes/glm/gtc/constants.hpp"

namespace Math {
	/// <summary> Returns an interpolated value between 4 corner values. </summary>
	/// <param name='dy'> 
	/// The amount interpolated in the y-direction (on the "width axis"). 
	/// Must be normalized to [0,1]. 
	/// </param>
	/// <param name='dz'> 
	/// The amount interpolated in the z-direction (on the "height axis"). 
	/// Must be normalized to [0,1]. 
	/// </param>
	/// <param name='x1'> The value of the "lower right" point. </param>
	/// <param name='x2'> The value of the "lower left" point. </param>
	/// <param name='x3'> The value of the "upper left" point. </param>
	/// <param name='x4'> The value of the "upper right" point. </param>
	float BilinearInterpolation(const float dy, const float dz,
								const float x1, const float x2,
								const float x3, const float x4);

	/// <summary> 
	/// A normal distribution generator that generates numbers between two given numbers.
	/// There will be a small peak at min and max (since normal distributions are not really 
	/// supposed to be limited to a range).
	/// </summary>
	class NormalDistributionGenerator {
	public:
		NormalDistributionGenerator(float min = 0.0f, float max = glm::half_pi<float>());
		float GetRandomFloat();
	private:
		std::normal_distribution<float> distribution;
		std::default_random_engine generator;
		float min, max;
	};

	/// <summary>
	/// Returns a vector that is non-parallell to a given vector.
	/// </summary>
	glm::vec3 NonParallellVector(const glm::vec3 & v);

	/// <summary> 
	/// Returns a hemisphere coordinate given two coordinates on a disk.
	/// </summary>
	glm::vec3 SampleHemisphereUsingDiskCoordinates(const float u, const float v);

	/// <summary>
	/// Returns a random direction given a normal.
	/// Uses cosine-weighted hemisphere sampling.
	/// </summary>
	glm::vec3 CosineWeightedHemisphereSampleDirection(const glm::vec3 & n);
}