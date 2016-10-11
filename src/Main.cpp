#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>

#include "Rendering\Camera.h"
#include "Utility\Math.h"
#include "Scene\SceneObjectFactory.h"
#include "PhotonMap\PhotonMap.h"

using namespace std;

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

///<summary> The main start for pengine. </summary>
int main()
{
	using cui = const unsigned int;

	auto RENDERING_MODE = RenderingMode::MONTE_CARLO;
	cui PIXELS_W = 400;
	cui PIXELS_H = 400;
	cui RAYS_PER_PIXEL = 512;
	cui MAX_RAY_DEPTH = 5;
	cui BOUNCES_PER_HIT = 1;
	cui PHOTONS_PER_LIGHT_SOURCE = 1000000;
	cui MAX_PHOTONS_PER_NODE = 1000;
	cui PHOTON_MAP_DEPTH = 5;
	float MAX_NODE_SIZE_DIMENSION = 0.1f;

	/* Initialize. */
	auto startTime = std::chrono::high_resolution_clock::now();
	std::cout << "Starting..." << std::endl;
	Camera c(PIXELS_W, PIXELS_H);

	/* Create scene. */
	std::cout << "Creating the scene..." << std::endl;
	Scene scene;
	SceneObjectFactory::AddRoom(scene, false);
	// SceneObjectFactory::AddSphere(scene, 10, 0, 2, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	// SceneObjectFactory::AddSphere(scene, 7, -3, 3, 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	// SceneObjectFactory::AddOrenNayarSphere(scene, 6, 3, 0, 1.5f, glm::vec3(1.0f, 0.35f, 1.0f));

	SceneObjectFactory::AddSphere(scene, 5, -3, 1.5f, 1.25f, glm::vec3(1.0f, 0.0f, 0.0f));
	SceneObjectFactory::AddSphere(scene, 6, 0, -0.5f, 1.25f, glm::vec3(0.0f, 1.0f, 0.0f));
	SceneObjectFactory::AddSphere(scene, 5, 3, -2, 1.25f, glm::vec3(0.0f, 0.0f, 1.0f));

	// SceneObjectFactory::AddTetrahedron(scene, 7, 0, 3, glm::vec3(1.0f, 1.0f, 1.0f));

	/* Lights. */
	SceneObjectFactory::Add2DQuad(scene, glm::vec2(1.5f, -1), glm::vec2(3.5f, 1), 3.96f,
								  glm::vec3(0, 0, -1), glm::vec3(1, 1, 1), glm::vec3(1.f, 1.0f, 1.f));
	// SceneObjectFactory::AddEmissiveSphere(scene, -2, 0, 0, 0.5f, glm::vec3(1, 1, 1), glm::vec3(2, 2, 2));
	// SceneObjectFactory::AddEmissiveSphere(scene, 7, 2, 2, 0.5f, glm::vec3(1, 1, 1), glm::vec3(1.0f, 0.35f, 0.55f));

	/* Initialize scene. */
	scene.Initialize();

	/* Generate PhotonMap. */
	c.GeneratePhotonMap(scene, PHOTONS_PER_LIGHT_SOURCE, MAX_PHOTONS_PER_NODE,
						MAX_NODE_SIZE_DIMENSION, PHOTON_MAP_DEPTH);

	/* Render scene. */
	c.Render(scene, RENDERING_MODE, RAYS_PER_PIXEL, MAX_RAY_DEPTH, BOUNCES_PER_HIT, glm::vec3(-7, 0, 0));

	/* Finalize. */
	auto timeElapsed = chrono::high_resolution_clock::now() - startTime;
	double took = chrono::duration_cast<std::chrono::milliseconds>(timeElapsed).count() / 1000.0;

	/* Write to files. */
	auto curdt = CurrentDateTime();
	const string imageFileName = /*"output/output_image.tga"; //*/"output/" + curdt + ".tga";
	const string textFileName = /*"output/output_image.txt"; //*/ "output/" + curdt + ".txt";
	c.WriteImageToTGA(imageFileName);
	ofstream out(textFileName);
	const unsigned int COL_WIDTH = 30;
	string renderingModeName;
	switch (RENDERING_MODE) {
	case RenderingMode::MONTE_CARLO:
		renderingModeName = "Monte Carlo";
		break;
	case RenderingMode::VISUALIZE_PHOTON_MAP:
		renderingModeName = "Visualize Photon Map";
		break;
	default: renderingModeName = "Unknown"; break;
	}
	out << setw(COL_WIDTH) << left << "Rendering mode:" << renderingModeName << endl;
	out << setw(COL_WIDTH) << left << "Dimensions:" << PIXELS_W << "x" << PIXELS_H << " pixels. " << endl;
	out << setw(COL_WIDTH) << left << "Rays per pixel:" << RAYS_PER_PIXEL << endl;
	out << setw(COL_WIDTH) << left << "Max ray depth:" << MAX_RAY_DEPTH << endl;
	out << setw(COL_WIDTH) << left << "Bounces per hit:" << BOUNCES_PER_HIT << endl;
	out << setw(COL_WIDTH) << left << "Render time:" << took << " seconds." << endl;
	out << setw(COL_WIDTH) << left << "Photons per light source:" << PHOTONS_PER_LIGHT_SOURCE << endl;
	out << setw(COL_WIDTH) << left << "Max photons per node:" << MAX_PHOTONS_PER_NODE << endl;
	out << setw(COL_WIDTH) << left << "Photon map depth:" << PHOTON_MAP_DEPTH << endl;
	out << setw(COL_WIDTH) << left << "Node box max dimension:" << MAX_NODE_SIZE_DIMENSION << " seconds." << endl;
	out.close();

	/* Finished. */
	std::cout << "Render saved to: " << imageFileName << "." << endl;
	std::cout << "Info saved to: " << imageFileName << "." << endl;
	std::cout << "Rendering finished... press any key to exit." << std::endl;
	std::cin.get();

	return 0;
}

