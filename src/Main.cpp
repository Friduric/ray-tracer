#pragma once

#include <iostream>
#include "Rendering\Camera.h"
#include "Utility\Math.h"
#include "Scene\SceneObjectFactory.h"

using namespace std;

///<summary> The main start for pengine. </summary>
int main()
{
	std::cout << "Starting..." << std::endl;

	/* Initialize camera. */
	Camera c(255, 255);

	/* Create scene. */
	Scene scene;
	SceneObjectFactory::AddRoom(scene, false);
	SceneObjectFactory::AddSphere(scene, 10, 0, 2, 1.0f, glm::vec3(1.0f, 0.15f, 0.5f));
	SceneObjectFactory::AddSphere(scene, 7, -3, 3, 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	SceneObjectFactory::AddOrenNayarSphere(scene, 6, 3, 0, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
	SceneObjectFactory::AddSphere(scene, 3, -3, 0, 1.25f, glm::vec3(1.0f, 0.0f, 1.0f));
	// SceneObjectFactory::AddTetrahedron(scene, 3, 0, 0, glm::vec3(0, 1.0f, 1.0f));
	SceneObjectFactory::AddEmissiveSphere(scene, 7, -2, -2, 0.5f, glm::vec3(1, 1, 1), glm::vec3(1, 1, 1));
	SceneObjectFactory::AddEmissiveSphere(scene, 7, 2, 2, 0.5f, glm::vec3(1, 1, 1), glm::vec3(1.0f, 0.0f, 0.0f));
	scene.Initialize();

	/* Render. */
	c.Render(scene, 32, glm::vec3(-6, 0, 0)); // Keep number of rays N so that you can write N as N = X^2 for some integer X.
	c.WriteImageToTGA();

	std::cout << "Rendering finished... press any key to exit." << std::endl;
	std::cin.get();

	return 0;
}

