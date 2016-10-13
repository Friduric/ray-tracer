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
#include "PhotonMap\PhotonMap.h" // TODO: Remove this.

using namespace std; // TODO: Remove this.

// Returns a tring that has information about current date and time.
std::string CurrentDateTime() {
	auto now = time(0);
	struct tm tstruct;
	localtime_s(&tstruct, &now);
	string date = std::to_string(tstruct.tm_year) + "-" +
		std::to_string(tstruct.tm_mon) + "-" + std::to_string(tstruct.tm_mday);
	string time = std::to_string(tstruct.tm_hour) + "-" +
		std::to_string(tstruct.tm_min) + "-" + std::to_string(tstruct.tm_sec);
	return date + "___" + time;
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
	cui RAYS_PER_PIXEL = 512;
	cui MAX_RAY_DEPTH = 4;
	cui BOUNCES_PER_HIT = 1;
	cui PHOTONS_PER_LIGHT_SOURCE = 500000;
	cui MIN_PHOTONS_PER_NODE = 10;
	cui PHOTON_MAP_DEPTH = 5;
	RendererType RENDERER_TYPE = RendererType::MONTE_CARLO;

	// --------------------------------------
	// Create the scene.
	// --------------------------------------
	Scene scene;
	std::cout << "Creating the scene..." << std::endl;

	SceneObjectFactory::AddRoom(scene, false);
	// SceneObjectFactory::AddSphere(scene, 10, 0, 2, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	// SceneObjectFactory::AddSphere(scene, 7, -3, 3, 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	// SceneObjectFactory::AddOrenNayarSphere(scene, 6, 3, 0, 1.5f, glm::vec3(1.0f, 0.35f, 1.0f));

	SceneObjectFactory::AddSphere(scene, 5, -3, 1.5f, 1.25f, glm::vec3(1.0f, 0.0f, 0.0f));
	SceneObjectFactory::AddSphere(scene, 6, 0, -0.5f, 1.25f, glm::vec3(0.0f, 1.0f, 0.0f));
	// SceneObjectFactory::AddSphere(scene, 5, 3, -2, 1.25f, glm::vec3(0.0f, 0.0f, 1.0f));
	SceneObjectFactory::AddTransparentSphere(scene, 5, 3, -2, 1.25f, glm::vec3(1.0f, 1.0f, 1.0f), 1.52f, 0.001f, 0.99f); // Mirror.
	SceneObjectFactory::AddTransparentSphere(scene, 3, -3, 3, 1.00f, glm::vec3(0.5f, 1.0f, 1.0f), 2.52f, 0.97f, 0.15f); // "Diamond".
	// SceneObjectFactory::AddTetrahedron(scene, 7, 0, 3, glm::vec3(1.0f, 1.0f, 1.0f));

	// Lights.
	SceneObjectFactory::Add2DQuad(scene, glm::vec2(5.0f, -1), glm::vec2(7.0f, 1), 4.99999f,
								  glm::vec3(0, 0, -1), glm::vec3(1, 1, 1), 1.0f);
	//SceneObjectFactory::AddEmissiveSphere(scene, 0, 2, 1, 0.5f, glm::vec3(1, 1, 1), 1.0f);
	//SceneObjectFactory::AddEmissiveSphere(scene, 7, 2, 2, 0.5f, glm::vec3(1, 1, 1), glm::vec3(1.0f, 0.35f, 0.55f));

	// --------------------------------------
	// Initialize camera and time keeping.
	// --------------------------------------
	scene.Initialize();
	auto startTime = std::chrono::high_resolution_clock::now();
	std::cout << "Starting..." << std::endl;
	Camera camera(PIXELS_W, PIXELS_H);

	// --------------------------------------
	// Render scene.
	// --------------------------------------
	Renderer * renderer = nullptr;
	switch (RENDERER_TYPE) {
	case RendererType::MONTE_CARLO:
		renderer = new MonteCarloRenderer(scene, MAX_RAY_DEPTH, BOUNCES_PER_HIT);
		break;
	case RendererType::PHOTON_MAP:
		renderer = new PhotonMapRenderer(scene, MAX_RAY_DEPTH, BOUNCES_PER_HIT, PHOTONS_PER_LIGHT_SOURCE, MIN_PHOTONS_PER_NODE, PHOTON_MAP_DEPTH);
		break;
	case RendererType::PHOTON_MAP_VISUALIZATION:
		renderer = new PhotonMapVisualizer(scene, PHOTONS_PER_LIGHT_SOURCE, MIN_PHOTONS_PER_NODE, PHOTON_MAP_DEPTH);
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
	auto timeElapsed = chrono::high_resolution_clock::now() - startTime;
	double took = chrono::duration_cast<std::chrono::milliseconds>(timeElapsed).count() / 1000.0;

	// --------------------------------------
	// Write render to file.
	// --------------------------------------
	auto currentDate = CurrentDateTime();
	const string imageFileName = "output/" + currentDate + ".tga";
	camera.WriteImageToTGA(imageFileName);

	// --------------------------------------
	// Write text data to file.
	// --------------------------------------
	const string textFileName = "output/" + currentDate + ".txt";
	ofstream out(textFileName);

	const unsigned int COL_WIDTH = 30;

	out << "-- RENDERING SETTINGS --" << endl;
	out << setw(COL_WIDTH) << left << "Rendering mode:" << renderer->RENDERER_NAME << endl;
	out << setw(COL_WIDTH) << left << "Dimensions:" << PIXELS_W << "x" << PIXELS_H << " pixels. " << endl;
	out << setw(COL_WIDTH) << left << "Rays per pixel:" << RAYS_PER_PIXEL << endl;
	out << setw(COL_WIDTH) << left << "Max ray depth:" << MAX_RAY_DEPTH << endl;
	out << setw(COL_WIDTH) << left << "Bounces per hit:" << BOUNCES_PER_HIT << endl;
	out << endl << "-- PHOTON MAP SETTINGS --" << endl;
	out << setw(COL_WIDTH) << left << "Photons per light source:" << PHOTONS_PER_LIGHT_SOURCE << endl;
	out << setw(COL_WIDTH) << left << "Max photons per node:" << MIN_PHOTONS_PER_NODE << endl;
	out << setw(COL_WIDTH) << left << "Photon map depth:" << PHOTON_MAP_DEPTH << endl;
	out << endl << "-- RENDERING STATISTICS --" << endl;
	out << setw(COL_WIDTH) << left << "Total time:" << took << " seconds." << endl;
	out << setw(COL_WIDTH) << left << "Time per pixel ray:" << took / (double)(RAYS_PER_PIXEL * PIXELS_W * PIXELS_H) << " seconds." << endl;
	out.close();

	// --------------------------------------
	// Finished!
	// --------------------------------------
	std::cout << "Render saved to: " << imageFileName << "." << endl;
	std::cout << "Info saved to: " << imageFileName << "." << endl;
	std::cout << "Rendering finished... press any key to exit." << std::endl;
	std::cout << "\b" << std::flush;
	std::cin.get();

	return 0;
}

