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

int main()
{
	using cui = const unsigned int;

	// --------------------------------------
	// Settings.
	// --------------------------------------
	cui PIXELS_W = 200;
	cui PIXELS_H = 200;
	cui RAYS_PER_PIXEL = 1028;
	cui MAX_RAY_DEPTH = 5;

	/* Settings. Adjust these to alter rendering. */
	cui PIXELS_W = 200;
	cui PIXELS_H = 200;
	cui RAYS_PER_PIXEL = 8;
	cui MAX_RAY_DEPTH = 2;
	cui BOUNCES_PER_HIT = 1;
	cui PHOTONS_PER_LIGHT_SOURCE = 500000;
	cui MAX_PHOTONS_PER_NODE = 10;
	cui PHOTON_MAP_DEPTH = 5;
	Scene scene;
	MonteCarloRenderer renderer(scene, MAX_RAY_DEPTH, BOUNCES_PER_HIT);

	// --------------------------------------
	// Create the scene.
	// --------------------------------------
	std::cout << "Creating the scene..." << std::endl;

	SceneObjectFactory::AddRoom(scene, false);
	// SceneObjectFactory::AddSphere(scene, 10, 0, 2, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	// SceneObjectFactory::AddSphere(scene, 7, -3, 3, 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	// SceneObjectFactory::AddOrenNayarSphere(scene, 6, 3, 0, 1.5f, glm::vec3(1.0f, 0.35f, 1.0f));

	SceneObjectFactory::AddSphere(scene, 5, -3, 1.5f, 1.25f, glm::vec3(1.0f, 0.0f, 0.0f));
	SceneObjectFactory::AddSphere(scene, 6, 0, -0.5f, 1.25f, glm::vec3(0.0f, 1.0f, 0.0f));
	SceneObjectFactory::AddSphere(scene, 5, 3, -2, 1.25f, glm::vec3(0.0f, 0.0f, 1.0f));

	// SceneObjectFactory::AddTetrahedron(scene, 7, 0, 3, glm::vec3(1.0f, 1.0f, 1.0f));

	// Lights.
	SceneObjectFactory::Add2DQuad(scene, glm::vec2(1.5f, -1), glm::vec2(3.5f, 1), 3.96f,
								  glm::vec3(0, 0, -1), glm::vec3(1, 1, 1), 1.0f);
	SceneObjectFactory::AddEmissiveSphere(scene, 0, 2, 1, 0.5f, glm::vec3(1, 1, 1), 1.0f);
	//SceneObjectFactory::AddEmissiveSphere(scene, 7, 2, 2, 0.5f, glm::vec3(1, 1, 1), glm::vec3(1.0f, 0.35f, 0.55f));

   // --------------------------------------
   // Initialize camera and time keeping.
   // --------------------------------------
	scene.Initialize();
	auto startTime = std::chrono::high_resolution_clock::now();
	std::cout << "Starting..." << std::endl;
	Camera camera(PIXELS_W, PIXELS_H);

	// --------------------------------------
	// Generate PhotonMap.
	// --------------------------------------
	// TODO: Move this to the renderers.
	scene.GeneratePhotonMap(PHOTONS_PER_LIGHT_SOURCE, MAX_PHOTONS_PER_NODE, PHOTON_MAP_DEPTH);

	// --------------------------------------
	// Render scene.
	// --------------------------------------
	camera.Render(scene, renderer, RAYS_PER_PIXEL, glm::vec3(-7, 0, 0));

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

	out << setw(COL_WIDTH) << left << "Rendering mode:" << renderer.RENDERER_NAME << endl;
	out << setw(COL_WIDTH) << left << "Dimensions:" << PIXELS_W << "x" << PIXELS_H << " pixels. " << endl;
	out << setw(COL_WIDTH) << left << "Rays per pixel:" << RAYS_PER_PIXEL << endl;
	out << setw(COL_WIDTH) << left << "Max ray depth:" << MAX_RAY_DEPTH << endl;
	out << setw(COL_WIDTH) << left << "Bounces per hit:" << BOUNCES_PER_HIT << endl;
	out << setw(COL_WIDTH) << left << "Render time:" << took << " seconds." << endl;
	out << setw(COL_WIDTH) << left << "Photons per light source:" << PHOTONS_PER_LIGHT_SOURCE << endl;
	out << setw(COL_WIDTH) << left << "Max photons per node:" << MAX_PHOTONS_PER_NODE << endl;
	out << setw(COL_WIDTH) << left << "Photon map depth:" << PHOTON_MAP_DEPTH << endl;
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

