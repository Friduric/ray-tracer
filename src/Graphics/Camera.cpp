#include "Camera.h"

#include <iostream>
#include <fstream>
#include <random>

#include "../Geometry/Ray.h"

Camera::Camera(const int _width, const int _height) : width(_width), height(_height) {
	pixels.assign(width, std::vector<Pixel>(height));
	discretizedPixels.assign(width, std::vector<glm::u8vec3>(height));
}

/// <param name='eye'> The eye of the viewer. </param>
/// <param name='c1'> Lower right corner of the camera plane. </param>
/// <param name='c2'> Lower left corner of the camera plane. </param>
/// <param name='c3'> Upper left corner of the camera plane. </param>
/// <param name='c4'> Upper right corner of the camera plane. </param>
void Camera::Render(const glm::vec3 eye, const glm::vec3 c1, const glm::vec3 c2,
					const glm::vec3 c3, const glm::vec3 c4, const float rayLength) {

	std::cout << "Rendering the scene..." << std::endl;

	std::random_device rd;
	std::uniform_real_distribution<float> rand(0, 1.0f);
	std::default_random_engine rengine(rd());

	/* Shoot a bunch of rays through the pixels. */
	float planeWidth = c4.y - c2.y;
	float planeHeight = c4.z - c2.z;

	float invWidth = 1.0f / (float)width;
	float invHeight = 1.0f / (float)height;
	float invPlaneWidth = 1.0f / (float)planeWidth;
	float invPlaneHeight = 1.0f / (float)planeHeight;

	float dy = planeWidth * invWidth;
	float dz = planeHeight * invHeight;

	for (unsigned int y = 0, float cy = -0.5f * planeWidth; y < width; ++y, cy += dy) {
		for (unsigned int z = 0, float cz = -0.5f * planeHeight; z < height; ++z, cz += dz) {
			/* Randomized offset in y and z. */
			float ry = rand(rengine) * dy;
			float rz = rand(rengine) * dz;

			/* Calculate infinitesimal dy and dz offset based on ry and rz. */
			float ddy = ry * invPlaneWidth;
			float ddz = rz * invPlaneHeight;

			/* Calculate x contribution from y. */
			float interpy = (y + ddy) * invWidth;
			float invy = 1.0f - interpy;
			float interpyplus = (y + ddy + 1) * invWidth;
			float invyplus = 1.0f - interpyplus;
			float nxy = invy * (c2.x + c3.x) + interpy * (c1.x + c4.x);
			float nxyplus = invyplus * (c2.x + c3.x) + interpyplus * (c1.x + c4.x);

			/* Calculate x contribution from z. */
			float interpz = (z + ddz) * invHeight;
			float invz = 1.0f - interpz;
			float interpzplus = (z + 1 + ddz) * invWidth;
			float invzplus = 1.0f - interpzplus;
			float nxz = invz * (c1.x + c2.x) + interpz * (c3.x + c4.x);
			float nxzplus = invzplus * (c1.x + c2.x) + interpzplus * (c3.x + c4.x);

			/* Calculate ray origin in plane. */
			float nx = 0.25f * (nxy + nxz);
			float ny = cy + ry;
			float nz = cz + rz;

			/* Create ray. */
			glm::vec3 from(nx, ny, nz);
			glm::vec3 direction = glm::normalize(from - eye);
			Ray ray(eye, from, from + rayLength * direction);
		}
	}

	/* Create the final discretized image from float values. */
	CreateImage(); // Should always be done immediately after the rendering step.
}

void Camera::CreateImage() {
	std::cout << "Creating raw pixel image..." << std::endl;

	// Find max color intensity. Could try using r + g + b instead.
	float maxIntensity = 0;
	for (size_t i = 0; i < width; ++i) {
		for (size_t j = 0; j < height; ++j) {
			maxIntensity = glm::max(maxIntensity, pixels[i][j].color.r);
			maxIntensity = glm::max(maxIntensity, pixels[i][j].color.g);
			maxIntensity = glm::max(maxIntensity, pixels[i][j].color.b);
		}
	}

	// TODO: Change this to some other way of detecting whether the image is dark (with spots).
	if (maxIntensity > BRIGHTNESS_DISCRETIZATION_THRESHOLD) {
		for (size_t i = 0; i < width; ++i) {
			for (size_t j = 0; j < height; ++j) {
				pixels[i][j].color.r = sqrt(pixels[i][j].color.r);
				pixels[i][j].color.g = sqrt(pixels[i][j].color.g);
				pixels[i][j].color.b = sqrt(pixels[i][j].color.b);
			}
		}
	}

	// Discretize pixels using the max intensity. Every value must be between 0 and 255.
	float f = maxIntensity < 0.00001 ? 0 : 254.99 / maxIntensity;
	for (size_t i = 0; i < width; ++i) {
		for (size_t j = 0; j < height; ++j) {
			float r = pixels[i][j].color.r * f;
			assert(r >= -FLT_EPSILON && r <= 256 - FLT_EPSILON);
			float g = pixels[i][j].color.g * f;
			assert(g >= -FLT_EPSILON && g <= 256 - FLT_EPSILON);
			float b = pixels[i][j].color.b * f;
			assert(b >= -FLT_EPSILON && b <= 256 - FLT_EPSILON);
			discretizedPixels[i][j].r = (glm::u8)round(r);
			discretizedPixels[i][j].g = (glm::u8)round(g);
			discretizedPixels[i][j].b = (glm::u8)round(b);
		}
	}
}

bool Camera::WriteImageToTGA(const std::string path) {
	std::cout << "Writing image to TGA..." << std::endl;

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