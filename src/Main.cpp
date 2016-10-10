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

	cui PIXELS_W = 200;
	cui PIXELS_H = 200;
	cui RAYS_PER_PIXEL = 256;
	cui MAX_RAY_DEPTH = 4;
	cui BOUNCES_PER_HIT = 1;

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
	SceneObjectFactory::AddOrenNayarSphere(scene, 6, 3, 0, 1.5f, glm::vec3(1.0f, 0.35f, 1.0f));
	SceneObjectFactory::AddSphere(scene, 3, -3, 0, 1.25f, glm::vec3(1.0f, 0.0f, 0.0f));
	SceneObjectFactory::AddSphere(scene, 3, 0, -2, 1.25f, glm::vec3(0.0f, 1.0f, 0.0f));
	// SceneObjectFactory::AddSphere(scene, 3, 3, -4, 1.25f, glm::vec3(0.0f, 0.0f, 1.0f));
	// SceneObjectFactory::AddTetrahedron(scene, 3, 0, 0, glm::vec3(1.0f, 1.0f, 1.0f));

	/* Lights. */
	SceneObjectFactory::Add2DQuad(scene, glm::vec2(1.5f, -1), glm::vec2(3.5f, 1), 3.96f,
								  glm::vec3(0, 0, -1), glm::vec3(1, 1, 1), glm::vec3(1.f, 0.86f, 0.78f));
	// SceneObjectFactory::AddEmissiveSphere(scene, -2, 0, 0, 0.5f, glm::vec3(1, 1, 1), glm::vec3(2, 2, 2));
	// SceneObjectFactory::AddEmissiveSphere(scene, 7, 2, 2, 0.5f, glm::vec3(1, 1, 1), glm::vec3(1.0f, 0.35f, 0.55f));

	/* Initialize scene. */
	scene.Initialize();

	/* Render scene. */
	c.Render(scene, RAYS_PER_PIXEL, MAX_RAY_DEPTH, BOUNCES_PER_HIT, glm::vec3(-7, 0, 0)); // Keep number of rays N so that you can write N as N = X^2 for some integer X.

	/* Finalize. */
	auto timeElapsed = chrono::high_resolution_clock::now() - startTime;
	double took = chrono::duration_cast<std::chrono::milliseconds>(timeElapsed).count() / 1000.0;

	/* Write to files. */
	auto curdt = CurrentDateTime();
	const string imageFileName = /*"output/output_image.tga"; //*/"output/" + curdt + ".tga";
	const string textFileName = /*"output/output_image.txt"; //*/ "output/" + curdt + ".txt";
	c.WriteImageToTGA(imageFileName);
	ofstream out(textFileName);
	const unsigned int COL_WIDTH = 21;
	out << setw(COL_WIDTH) << left << "Dimensions:" << PIXELS_W << "x" << PIXELS_H << " pixels. " << endl;
	out << setw(COL_WIDTH) << left << "Rays per pixel:" << RAYS_PER_PIXEL << endl;
	out << setw(COL_WIDTH) << left << "Max ray depth:" << MAX_RAY_DEPTH << endl;
	out << setw(COL_WIDTH) << left << "Bounces per hit:" << BOUNCES_PER_HIT << endl;
	out << setw(COL_WIDTH) << left << "Render time:" << took << " seconds." << endl;
	out.close();

	/* Finished. */
	std::cout << "Render saved to: " << imageFileName << "." << endl;
	std::cout << "Info saved to: " << imageFileName << "." << endl;
	std::cout << "Rendering finished... press any key to exit." << std::endl;
	std::cin.get();

	return 0;
}

