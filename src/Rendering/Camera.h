#pragma once

#include <vector>

#include <glm.hpp>

#include "../Scene/Scene.h"
#include "Renderers\Renderer.h"
#include "Pixel.h"

class Camera {
public:
	/// <summary> The width of the camera in pixels. </summary>
	unsigned int width;

	/// <summary> The height of the camera in pixels. </summary>
	unsigned int height;

	/// <summary> Constructs an image. </summary>
	/// <param name="width"> The width of the image in pixels. </param>
	/// <param name="height"> The height of the image in pixels. </param>
	Camera(const unsigned int width = 1000, const unsigned int height = 1000);

	/// <summary>
	/// Renders the image by setting the color of each pixel according to Monte Carlo 
	/// ray tracing techniques.
	/// </summary>
	/// <param name='scene'> The scene which we are going to render </param>
	/// <param name='eye'> The eye of the viewer. </param>
	/// <param name='c1'> Lower right corner of the camera plane. </param>
	/// <param name='c2'> Lower left corner of the camera plane. </param>
	/// <param name='c3'> Upper left corner of the camera plane. </param>
	/// <param name='c4'> Upper right corner of the camera plane. </param>
	/// <param name='RAY_LENGTH'> The length of all the rays used to render the scene. </param>
	/// <param name='RAYS_PER_PIXEL'> 
	/// The number of rays which we trace through each pixel. 
	/// Best results are given if RAYS_PER_PIXEL = N^2 for some integer N. 
	/// </param> 
	void Render(const Scene & scene, Renderer & renderer,
				const unsigned int RAYS_PER_PIXEL = 1024,
				const glm::vec3 eye = glm::vec3(-7, 0, 0),
				const glm::vec3 c1 = glm::vec3(-5, -1, -1), const glm::vec3 c2 = glm::vec3(-5, 1, -1),
				const glm::vec3 c3 = glm::vec3(-5, 1, 1), const glm::vec3 c4 = glm::vec3(-5, -1, 1));

	/// <summary> 
	/// Writes the discretized pixels to a TGA image.
	/// Returns true if successful. 
	/// </summary>
	bool WriteImageToTGA(const std::string path = "output/output_image.tga") const;
private:
	// Pixel containers.
	std::vector<std::vector<Pixel>> pixels;
	std::vector<std::vector<glm::u8vec3>> discretizedPixels;

	/// <summary> Discretizes the color of each pixel. </summary>
	void CreateImage(const float BRIGHTNESS_DISCRETIZATION_THRESHOLD = 1.25f);
};