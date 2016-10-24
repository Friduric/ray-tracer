#include "Camera.h"

#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>

#include "../Geometry/Ray.h"
#include "../Utility/Math.h"

#define __LOG_TIME_INTERVAL 3 // In seconds. 
#define __USE_PARALLELIZATION false // Whether to use multiple threads for rendering or not.

Camera::Camera(const unsigned int _width, const unsigned int _height) :
	width(_width), height(_height) {
	pixels.assign(width, std::vector<Pixel>(height));
	discretizedPixels.assign(width, std::vector<glm::u8vec3>(height));
}

void Camera::Render(const Scene & scene, Renderer & renderer, const unsigned int RAYS_PER_PIXEL,
					const glm::vec3 eye, const glm::vec3 c1, const glm::vec3 c2,
					const glm::vec3 c3, const glm::vec3 c4) {

	std::cout << std::endl << "Rendering the scene ..." << std::endl;
	const auto startTime = std::chrono::high_resolution_clock::now();

	// Initalize the random engine.
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_real_distribution<float> rand(0, 1.0f - FLT_EPSILON);

	// Precompute inverse widths and heights.
	const float INV_WIDTH = 1.0f / static_cast<float>(width);
	const float INV_HEIGHT = 1.0f / static_cast<float>(height);
	const float INV_RAYS_PER_PIXEL = 1.0f / static_cast<float>(RAYS_PER_PIXEL);

	// Calculate step lengths.
	const float SQRT_QUADS_PER_PIXEL = sqrtf(static_cast<float>(RAYS_PER_PIXEL));
	const float INV_SQRT_QUADS_PER_PIXEL = 1.0f / SQRT_QUADS_PER_PIXEL;
	const float COLUMN_PIXEL_STEP = INV_WIDTH * INV_SQRT_QUADS_PER_PIXEL;
	const float ROW_PIXEL_STEP = INV_HEIGHT * INV_SQRT_QUADS_PER_PIXEL;

	// Camera plane normal.
	const glm::vec3 CAMERA_PLANE_NORMAL = -glm::normalize(glm::cross(c1 - c2, c1 - c4));

	double timeSinceLastLog = 0.0;
	// Shoot multiple rays through every pixel.
	for (unsigned int y = 0; y < width; ++y) {
		const auto before = std::chrono::high_resolution_clock::now();

		// OMP doesn't allow unsigned int in for parallelized for loop.
#if __USE_PARALLELIZATION
#pragma omp parallel for schedule(static) // Parallelize using OMP.
#endif
		for (int z = 0; z < static_cast<int>(height); ++z) {

			// Shoot a bunch of rays through the pixel (y, z), and accumulate colors.
			Ray ray;
			glm::vec3 colorAccumulator = colorAccumulator = glm::vec3(0, 0, 0);
			for (float c = 0; c < INV_WIDTH - COLUMN_PIXEL_STEP + FLT_EPSILON; c += COLUMN_PIXEL_STEP) {
				for (float r = 0; r < INV_HEIGHT - ROW_PIXEL_STEP + FLT_EPSILON; r += ROW_PIXEL_STEP) {

					// Calculate camera plane ray position using stratified sampling.
					const float ylerp = y * INV_WIDTH + c + rand(gen) * COLUMN_PIXEL_STEP;
					const float zlerp = z * INV_HEIGHT + r + rand(gen) * ROW_PIXEL_STEP;
					const float nx = Utility::Math::BilinearInterpolation(ylerp, zlerp, c1.x, c2.x, c3.x, c4.x);
					const float ny = Utility::Math::BilinearInterpolation(ylerp, zlerp, c1.y, c2.y, c3.y, c4.y);
					const float nz = Utility::Math::BilinearInterpolation(ylerp, zlerp, c1.z, c2.z, c3.z, c4.z);

					// Create ray.
					ray.from = glm::vec3(nx, ny, nz);
					ray.direction = glm::normalize(ray.from - eye);
					const float rayFactor = std::max(0.0f, glm::dot(ray.from, CAMERA_PLANE_NORMAL));

					// Shoot ray.
					colorAccumulator += rayFactor * renderer.GetPixelColor(ray);
				}
			}

			// Set pixel color dependent on the traced ray.
			pixels[y][z].color = INV_RAYS_PER_PIXEL * colorAccumulator;
		}

		// Estimate time left.
		const auto now = std::chrono::high_resolution_clock::now();
		const double step = (double)std::chrono::duration_cast<std::chrono::milliseconds>(now - before).count();
		timeSinceLastLog += step * 0.001;
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
		const double percentageDone = 100 * (y / (double)width);
		const double percentageLeft = (100 - percentageDone);
		long long estimatedTimeLeft = (long long)llround((elapsedTime / percentageDone) * percentageLeft * 0.001);
		long long secs = estimatedTimeLeft % 60;
		long long mins = (estimatedTimeLeft / 60) % 60;
		long long hours = ((estimatedTimeLeft / 60) / 60);
		if (timeSinceLastLog > __LOG_TIME_INTERVAL) {
			timeSinceLastLog = 0.0;
			std::cout << std::setprecision(1) << std::fixed;
			std::cout << "Rendered " << percentageDone << "%. ";
			std::cout << "Time left is " << hours << " h., " << mins << "m. and " << secs << "s." << std::endl;
		}
	}

	const auto endTime = std::chrono::high_resolution_clock::now();
	const auto took = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
	std::cout << "Rendering finished and took: " << (took / 1000.0) << " seconds." << std::endl << std::endl;

	// Create the final discretized image. Should always be done immediately after the rendering step.
	CreateImage();
}

void Camera::CreateImage(const float BRIGHTNESS_DISCRETIZATION_THRESHOLD) {
	std::cout << "Creating a discretized image from the rendered image ..." << std::endl;

	// Find max color intensity.
	float maxIntensity = 0;
	for (size_t i = 0; i < width; ++i) {
		for (size_t j = 0; j < height; ++j) {
			const auto & c = pixels[i][j].color;
			maxIntensity = std::max<float>(c.r, maxIntensity);
			maxIntensity = std::max<float>(c.g, maxIntensity);
			maxIntensity = std::max<float>(c.b, maxIntensity);
		}
	}

	if (maxIntensity < FLT_EPSILON * 5.0f) {
		std::cerr << "Rendered image intensity was very low. Impossible to discretize image." << std::endl;
		return;
	}

	// Squash image.
	for (size_t i = 0; i < width; ++i) {
		for (size_t j = 0; j < height; ++j) {
			pixels[i][j].color = sqrt(pixels[i][j].color);
		}
	}
	maxIntensity = sqrt(maxIntensity);

	// Discretize pixels using the max intensity. Every discretized value must be between 0 and 255.
	glm::u8 discretizedMaxIntensity{};
	const float f = 254.99f / maxIntensity;
	for (size_t i = 0; i < width; ++i) {
		for (size_t j = 0; j < height; ++j) {
			const auto c = f * pixels[i][j].color;
			assert(c.r >= -FLT_EPSILON && c.r <= 255.5f - FLT_EPSILON);
			assert(c.g >= -FLT_EPSILON && c.g <= 255.5f - FLT_EPSILON);
			assert(c.b >= -FLT_EPSILON && c.b <= 255.5f - FLT_EPSILON);
			discretizedPixels[i][j].r = (glm::u8)round(c.r);
			discretizedPixels[i][j].g = (glm::u8)round(c.g);
			discretizedPixels[i][j].b = (glm::u8)round(c.b);
			discretizedMaxIntensity = glm::max(discretizedMaxIntensity, discretizedPixels[i][j].r);
			discretizedMaxIntensity = glm::max(discretizedMaxIntensity, discretizedPixels[i][j].g);
			discretizedMaxIntensity = glm::max(discretizedMaxIntensity, discretizedPixels[i][j].b);
		}
	}
	assert(discretizedMaxIntensity == 255); // Discretized max intensity failed.
	std::cout << "Image max intensity was: " << maxIntensity << std::endl;
}

bool Camera::WriteImageToTGA(const std::string path) const {

	// Initialize.
	std::cout << "Writing image to TGA ..." << std::endl;
	assert(width > 0 && height > 0);
	std::ofstream o(path.c_str(), std::ios::out | std::ios::binary);

	// Write header.
	const std::string header = "002000000000";
	for (unsigned int i = 0; i < header.length(); ++i) {
		o.put(header[i] - '0');
	}

	o.put(width & 0x00FF);
	o.put((width & 0xFF00) >> 8);
	o.put(width & 0x00FF);
	o.put((height & 0xFF00) >> 8);
	o.put(32); // 24 bit bitmap.
	o.put(0);

	// Write data.
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			auto& cp = discretizedPixels[x][y];
			o.put(cp.b);
			o.put(cp.g);
			o.put(cp.r);
			o.put((char)0xFF); // 255.
		}
	}

	o.close();

	return true;
}