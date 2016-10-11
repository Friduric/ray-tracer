#pragma once

#include <vector>

#include <glm.hpp>

#include "../Scene/Scene.h"
#include "Pixel.h"
#include "../PhotonMap/PhotonMap.h"

enum RenderingMode {
	VISUALIZE_PHOTON_MAP, MONTE_CARLO
};

class Camera {
public:
	size_t width, height;

	/// <summary> Constructs an image. </summary>
	/// <param name="width"> The width of the image in pixels. </param>
	/// <param name="height"> The height of the image in pixels. </param>
	Camera(const int width = 1000, const int height = 1000);

	/// <summary> 
	/// Generates a photonmap that can be used during rendering.
	/// </summary>
	/// <param name='scene'> The scene. </param>
	/// <param name='PHOTONS_PER_LIGHT_SOURCE'> The amount of photons used per light source. </param>
	/// <param name='MAX_PHOTONS_PER_NODE'> The maximum amount of photons per node. </param>
	/// <param name='MAXIMUM_NODE_BOX_DIMENSION'> The maximum width, height and depth of a nodes box size. </param>
	/// <param name='MAX_DEPTH'> The amount of times each photon will bounce at most. </param>
	void GeneratePhotonMap(const Scene & scene,
						   const unsigned int PHOTONS_PER_LIGHT_SOURCE = 200000,
						   const unsigned int MAX_PHOTONS_PER_NODE = 100,
						   const float MAXIMUM_NODE_BOX_DIMENSION = 0.1f,
						   const unsigned int MAX_DEPTH = 5);

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
	void Render(const Scene & scene, const RenderingMode RENDERING_MODE = RenderingMode::MONTE_CARLO,
				const unsigned int RAYS_PER_PIXEL = 1024, const unsigned int RAY_MAX_DEPTH = 5,
				const unsigned int RAY_MAX_BOUNCE = 1, const glm::vec3 eye = glm::vec3(-7, 0, 0),
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

	// PhotonMap
	PhotonMap photonMap;
};