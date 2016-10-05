#pragma once

#include "Pixel.h"
#include <glm.hpp>
#include <vector>

class Camera {
public:
	// Constructor.
	Camera(int width = PIXELS_WIDTH, int height = PIXELS_HEIGHT);
	size_t width, height;

	/// Renders the image by setting the color of each pixel according to Monte Carlo 
	/// ray tracing techniques.
	void Render();

	/// Writes the discretized pixels to a TGA image.
	/// Returns true if successful.
	bool WriteImageToTGA(std::string path = "output/output_image.tga");
private:
	// Constants.
	constexpr static double BRIGHTNESS_DISCRETIZATION_THRESHOLD = 1.25;
	const static unsigned int PIXELS_WIDTH = 1;
	const static unsigned int PIXELS_HEIGHT = 1;

	// Base vertices.
	glm::vec3 eye1;
	glm::vec3 eye2;

	// Pixel containers.
	std::vector<std::vector<Pixel>> pixels;
	std::vector<std::vector<glm::u8vec3>> discretizedPixels;

	/// Discretizes the color of each pixel.
	void CreateImage();
};