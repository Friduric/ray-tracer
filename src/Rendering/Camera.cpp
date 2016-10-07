#include "Camera.h"

#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>

#include "../Geometry/Ray.h"
#include "../Utility/Math.h"

#define __LOG_ITERATIONS

Camera::Camera(const int _width, const int _height) : width(_width), height(_height) {
	pixels.assign(width, std::vector<Pixel>(height));
	discretizedPixels.assign(width, std::vector<glm::u8vec3>(height));
}

void Camera::Render(const Scene & scene, const unsigned int RAYS_PER_PIXEL,
					const glm::vec3 eye,
					const glm::vec3 c1, const glm::vec3 c2,
					const glm::vec3 c3, const glm::vec3 c4,
					const float RAY_LENGTH) {

	std::cout << "Rendering the scene ..." << std::endl;

	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_real_distribution<float> rand(0, 1.0f - FLT_EPSILON);

	const float invWidth = 1.0f / (float)width;
	const float invHeight = 1.0f / (float)height;
	const float INV_RAYS_PER_PIXEL = 1.0f / static_cast<float>(RAYS_PER_PIXEL);

	/* Number of quads per pixel. */
	const unsigned int SQRT_QUADS_PER_PIXEL = (unsigned int)(sqrt(RAYS_PER_PIXEL) + 0.5f); // Round.
	const float INV_SQRT_QUADS_PER_PIXEL = 1.0f / (float)SQRT_QUADS_PER_PIXEL;

#ifdef __LOG_ITERATIONS
	long long ctr = 0;
#endif // __LOG_ITERATIONS

	/* For each pixel, shoot a bunch of rays through it. */
	for (unsigned int y = 0; y < width; ++y) {
		for (unsigned int z = 0; z < height; ++z) {
#ifdef __LOG_ITERATIONS
			if (++ctr % 100 == 0) {
				std::cout << ctr << "/" << width * height << " pixels." << std::endl;
			}
#endif // __LOG_ITERATIONS

			/* Shoot a bunch of rays through the pixel (y, z), and accumulate color. */
			glm::vec3 colorAccumulator = glm::vec3(0, 0, 0);

			for (unsigned int c = 0; c < SQRT_QUADS_PER_PIXEL; ++c) {
				float yy = c * invWidth * INV_SQRT_QUADS_PER_PIXEL;
				for (unsigned int r = 0; r < SQRT_QUADS_PER_PIXEL; ++r) {
					float zz = r * invHeight * INV_SQRT_QUADS_PER_PIXEL;

					/* Calculate new randomized point in the camera plane. */
					const float ylerp = (y + yy + rand(gen) * INV_SQRT_QUADS_PER_PIXEL) * invWidth;
					const float zlerp = (z + zz + rand(gen) * INV_SQRT_QUADS_PER_PIXEL) * invHeight;
					const float nx = Math::BilinearInterpolation(ylerp, zlerp, c1.x, c2.x, c3.x, c4.x);
					const float ny = Math::BilinearInterpolation(ylerp, zlerp, c1.y, c2.y, c3.y, c4.y);
					const float nz = Math::BilinearInterpolation(ylerp, zlerp, c1.z, c2.z, c3.z, c4.z);

					/* Create ray. */
					const glm::vec3 planePosition(nx, ny, nz); // The camera plane intersection position.
					const glm::vec3 rayDirection = glm::normalize(planePosition - eye);
					const Ray ray(planePosition, planePosition + RAY_LENGTH * rayDirection);

					/* Trace ray through the scene. */
					colorAccumulator += scene.TraceRay(ray);
				}
			}

			/* Set pixel color dependent on ray trace. */
			pixels[y][z].color = INV_RAYS_PER_PIXEL * colorAccumulator;
		}
	}

	/* Create the final discretized image from float the values. */
	CreateImage(); // Should always be done immediately after the rendering step.
}

void Camera::CreateImage(const float BRIGHTNESS_DISCRETIZATION_THRESHOLD) {
	std::cout << "Creating a discretized image from the rendered image ..." << std::endl;

	/* Find max color intensity. */
	float maxIntensity = 0;
	for (size_t i = 0; i < width; ++i) {
		for (size_t j = 0; j < height; ++j) {
			float rgb = pixels[i][j].color.r + pixels[i][j].color.g + pixels[i][j].color.b;
			maxIntensity = std::max<float>(rgb, maxIntensity);
		}
	}

	// TODO: Change this to some other way of detecting whether the image is dark (with spots).
	if (maxIntensity > BRIGHTNESS_DISCRETIZATION_THRESHOLD) {
		std::cout << "Squashing brightness color due to high brightness." << std::endl;
		for (size_t i = 0; i < width; ++i) {
			for (size_t j = 0; j < height; ++j) {
				pixels[i][j].color.r = sqrt(pixels[i][j].color.r);
				pixels[i][j].color.g = sqrt(pixels[i][j].color.g);
				pixels[i][j].color.b = sqrt(pixels[i][j].color.b);
			}
		}
		maxIntensity = sqrt(maxIntensity);
	}

	if (maxIntensity < FLT_EPSILON * 10.0f) {
		std::cout << "Image max intensity was very low." << std::endl;
	}

	// Discretize pixels using the max intensity. Every value must be between 0 and 255.
	float f = maxIntensity < FLT_EPSILON * 10.0f ? 0.0f : (3.0f * 254.99f) / maxIntensity;
	for (size_t i = 0; i < width; ++i) {
		for (size_t j = 0; j < height; ++j) {
			float r = pixels[i][j].color.r * f;
			assert(r >= -FLT_EPSILON && r <= 255.5f - FLT_EPSILON);
			float g = pixels[i][j].color.g * f;
			assert(g >= -FLT_EPSILON && g <= 255.5f - FLT_EPSILON);
			float b = pixels[i][j].color.b * f;
			assert(b >= -FLT_EPSILON && b <= 255.5f - FLT_EPSILON);
			discretizedPixels[i][j].r = (glm::u8)round(abs(r));
			discretizedPixels[i][j].g = (glm::u8)round(abs(g));
			discretizedPixels[i][j].b = (glm::u8)round(abs(b));
		}
	}
}

bool Camera::WriteImageToTGA(const std::string path) const {
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