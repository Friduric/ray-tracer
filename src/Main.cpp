#pragma once

// Stdlib.
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>

// Rendering.
#include "Rendering\Camera.h"
#include "Rendering\Renderers\Renderer.h"
#include "Rendering\Renderers\MonteCarloRenderer.h"
#include "Rendering\Renderers\PhotonMapRenderer.h"
#include "Rendering\Renderers\PhotonMapVisualizer.h"

// Other.
#include "Utility\Math.h"
#include "Scene\SceneObjectFactory.h"

namespace {
	// Returns a string that represents the current date and time.
	std::string CurrentDateTime() {
		auto now = time(0);
		struct tm tstruct;
		localtime_s(&tstruct, &now);
		std::string date = std::to_string(tstruct.tm_year) + "-" +
			std::to_string(tstruct.tm_mon) + "-" + std::to_string(tstruct.tm_mday);
		std::string time = std::to_string(tstruct.tm_hour) + "-" +
			std::to_string(tstruct.tm_min) + "-" + std::to_string(tstruct.tm_sec);
		return date + "___" + time;
	}
}

int main() {
	using cui = const unsigned int;
	enum RendererType {
		MONTE_CARLO, PHOTON_MAP, PHOTON_MAP_VISUALIZATION
	};

	// --------------------------------------
	// Settings.
	// --------------------------------------
	cui PIXELS_W = 400;
	cui PIXELS_H = 400;
	cui RAYS_PER_PIXEL = 8;
	cui MAX_RAY_DEPTH = 5;
	cui BOUNCES_PER_HIT = 1;
	cui PHOTONS_PER_LIGHT_SOURCE = 100000;
	cui PHOTON_MAP_DEPTH = 4;
	const RendererType RENDERER_TYPE = RendererType::PHOTON_MAP;

	// --------------------------------------
	// Create the scene.
	// --------------------------------------
	Scene scene;
	std::cout << "Creating the scene ..." << std::endl;

	// Coordinate system relative to camera plane.
	// +x is INTO the image.
	// +y is LEFT in image.
	// +z is UP in image. 

	// Add the room.
	SceneObjectFactory::AddRoom(scene, false);

	// Add the scene objects.
	SceneObjectFactory::AddOrenNayarSphere(scene, 5, -0.5f, -4.0f, 0.75f, glm::vec3(1.0f, 0.0f, 0.0f), 0.00f); //red oren
	SceneObjectFactory::AddSphere(scene, 10, 0, -0.5f, 1.25f, glm::vec3(1.0f, 0.2f, 1.0f)); //green diffuse
	SceneObjectFactory::AddSphere(scene, 6, 4, 2, 1.15f, glm::vec3(1.0f, 1.0f, 0.2f)); // yellow
	SceneObjectFactory::AddTransparentSphere(scene, 6.0f, -3.5f, -3.5f, 1.15f, glm::vec3(0.5f, 0.5f, 1.0f), 1.15f, 1.0f, 0.6f, 0.0f, 255.0f); // "thin glass" right.
	SceneObjectFactory::AddTransparentSphere(scene, 8.0f, 3.0f, -3.0f, 1.15f, glm::vec3(1.0f, 1.0f, 0.2f), 1.15f, 0.0f, 1.0f, 0.0f, 255.0f);//mirror
	SceneObjectFactory::AddTransparentSphere(scene, 6, -4.0f, 1.75f, 1.5f, glm::vec3(1.0f, 1.0f, 0.0f), 1.47f, 1.0f, 0.0f, 1.0f, 255.0f); // "Glass".
	SceneObjectFactory::AddTransparentSphere(scene, 7, 0.0f, 2.75f, 0.5f, glm::vec3(0.8f, 0.2f, 1.0f), 1.37f, 1.0f, 0.0f, 1.0f, 255.0f);
	SceneObjectFactory::AddTransparentSphere(scene, 6.5f, -2.0f, 0.95f, 1.0f, glm::vec3(0.7f, 1.0f, 0.0f), 1.47f, 1.0f, 0.0f, 1.0f, 255.0f);
	SceneObjectFactory::AddTetrahedron(scene, 7, 4, -1, glm::vec3(0.0f, 1.0f, 1.0f), 0.0f, 1.0f, 0.0f, 0.0f);

	// Lights.
	SceneObjectFactory::Add2DQuad(scene, glm::vec2(5.0f, -1), glm::vec2(7.0f, 1), 4.99999f, glm::vec3(0, 0, -1), glm::vec3(1, 1.0f, 1.0f), 1.0f);
	SceneObjectFactory::Add2DQuad(scene, glm::vec2(4.0f, 2), glm::vec2(5.0f, 3), 4.99999f, glm::vec3(0, 0, -1), glm::vec3(1.0f, 1.0f, 0.65f), 1.0f);

	// --------------------------------------
	// Initialize camera and time keeping.
	// --------------------------------------
	std::cout << "Initializing the camera and the scene ..." << std::endl;
	scene.Initialize();
	auto startTime = std::chrono::high_resolution_clock::now();
	Camera camera(PIXELS_W, PIXELS_H);

	// --------------------------------------
	// Render scene.
	// --------------------------------------
	Renderer * renderer = nullptr;
	switch (RENDERER_TYPE) {
	case RendererType::MONTE_CARLO:
		renderer = new MonteCarloRenderer(scene, MAX_RAY_DEPTH);
		break;
	case RendererType::PHOTON_MAP:
		renderer = new PhotonMapRenderer(scene, MAX_RAY_DEPTH, BOUNCES_PER_HIT, PHOTONS_PER_LIGHT_SOURCE, PHOTON_MAP_DEPTH);
		break;
	case RendererType::PHOTON_MAP_VISUALIZATION:
		renderer = new PhotonMapVisualizer(scene, PHOTONS_PER_LIGHT_SOURCE, PHOTON_MAP_DEPTH);
		break;
	}
	if (renderer == nullptr) {
		std::cerr << "Failed to initialize renderer." << std::endl;
		return 0;
	}
	camera.Render(scene, *renderer, RAYS_PER_PIXEL, glm::vec3(-7, 0, 0));

	// --------------------------------------
	// Finalize.
	// --------------------------------------
	auto timeElapsed = std::chrono::high_resolution_clock::now() - startTime;
	double took = std::chrono::duration_cast<std::chrono::milliseconds>(timeElapsed).count() / 1000.0;

	// --------------------------------------
	// Write render to file.
	// --------------------------------------
	auto currentDate = CurrentDateTime();
	const std::string imageFileName = "output/" + currentDate + ".tga";
	camera.WriteImageToTGA(imageFileName);

	// --------------------------------------
	// Write text data to file.
	// --------------------------------------
	const std::string textFileName = "output/" + currentDate + ".txt";
	std::ofstream out(textFileName);

	const unsigned int COL_WIDTH = 30;

	out << "-- RENDERING SETTINGS --" << std::endl;
	out << std::setw(COL_WIDTH) << std::left << "Rendering mode:" << renderer->RENDERER_NAME << std::endl;
	out << std::setw(COL_WIDTH) << std::left << "Dimensions:" << PIXELS_W << "x" << PIXELS_H << " pixels. " << std::endl;
	out << std::setw(COL_WIDTH) << std::left << "Rays per pixel:" << RAYS_PER_PIXEL << std::endl;
	out << std::setw(COL_WIDTH) << std::left << "Max ray depth:" << MAX_RAY_DEPTH << std::endl;
	out << std::setw(COL_WIDTH) << std::left << "Bounces per hit:" << BOUNCES_PER_HIT << std::endl;
	out << std::endl << "-- PHOTON MAP SETTINGS --" << std::endl;
	out << std::setw(COL_WIDTH) << std::left << "Photons per light source:" << PHOTONS_PER_LIGHT_SOURCE << std::endl;
	out << std::setw(COL_WIDTH) << std::left << "Photon map depth:" << PHOTON_MAP_DEPTH << std::endl;
	out << std::endl << "-- RENDERING STATISTICS --" << std::endl;
	out << std::setw(COL_WIDTH) << std::left << "Total time:" << took << " seconds." << std::endl;
	out << std::setw(COL_WIDTH) << std::left << "Time per pixel ray:" << took / (double)(RAYS_PER_PIXEL * PIXELS_W * PIXELS_H) << " seconds." << std::endl;
	out.close();

	// --------------------------------------
	// Finished!
	// --------------------------------------
	std::cout << "Render saved to: " << imageFileName << "." << std::endl;
	std::cout << "Info saved to: " << imageFileName << "." << std::endl;
	std::cout << "Rendering finished... press any key to exit." << std::endl;
	std::cout << "\b" << std::flush;
	std::cin.get();

	return 0;
}

