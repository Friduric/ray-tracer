#include "Camera.h"

#include <iostream>
#include <fstream>

Camera::Camera(int _width, int _height) : width(_width), height(_height) {
	pixels.assign(width, std::vector<Pixel>(height));
	discretizedPixels.assign(width, std::vector<glm::u8vec3>(height));
}

void Camera::Render() {
	std::cout << "Rendering the scene..." << std::endl;
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

bool Camera::WriteImageToTGA(std::string path) {
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