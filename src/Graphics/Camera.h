#pragma once

#include "Pixel.h"
#include <glm.hpp>
#include <vector>

class Camera {
public:
	Camera(const int width = PIXELS_WIDTH, const int height = PIXELS_HEIGHT);
	size_t width, height;

	/// <summary>
	/// Renders the image by setting the color of each pixel according to Monte Carlo 
	/// ray tracing techniques.
	/// </summary>
	/// <param name='eye'> The eye of the viewer. </param>
	/// <param name='c1'> Lower right corner of the camera plane. </param>
	/// <param name='c2'> Lower left corner of the camera plane. </param>
	/// <param name='c3'> Upper left corner of the camera plane. </param>
	/// <param name='c4'> Upper right corner of the camera plane. </param>
	void Render(const glm::vec3 eye = glm::vec3(-2, 0, 0),
				const glm::vec3 c1 = glm::vec3(0, -1, -1), const glm::vec3 c2 = glm::vec3(0, 1, -1),
				const glm::vec3 c3 = glm::vec3(0, 1, 1), const glm::vec3 c4 = glm::vec3(0, -1, 1),
				const float RAY_LENGTH = 1000.0f, const float RAYS_PER_PIXEL = 5);

	/// <summary> 
	/// Writes the discretized pixels to a TGA image.
	/// Returns true if successful. 
	/// </summary>
	bool WriteImageToTGA(const std::string path = "output/output_image.tga");
private:
	// Constants.
	constexpr static double BRIGHTNESS_DISCRETIZATION_THRESHOLD = 1.25;
	const static unsigned int PIXELS_WIDTH = 1;
	const static unsigned int PIXELS_HEIGHT = 1;

	// Pixel containers.
	std::vector<std::vector<Pixel>> pixels;
	std::vector<std::vector<glm::u8vec3>> discretizedPixels;

	/// <summary> Discretizes the color of each pixel. </summary>
	void CreateImage();
};