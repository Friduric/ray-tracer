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
	Camera c(400, 400);

	std::cout << "Creating the scene..." << std::endl;

	/* Create scene. */
	Scene scene;
	SceneObjectFactory::AddRoom(scene, false);
	// SceneObjectFactory::AddSphere(scene, 10, 0, 2, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	// SceneObjectFactory::AddSphere(scene, 7, -3, 3, 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	// SceneObjectFactory::AddOrenNayarSphere(scene, 6, 3, 0, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
	SceneObjectFactory::AddSphere(scene, 3, -3, 0, 1.25f, glm::vec3(1.0f, 0.0f, 0.0f));
	SceneObjectFactory::AddSphere(scene, 3, 0, 0, 1.25f, glm::vec3(0.0f, 1.0f, 0.0f));
	SceneObjectFactory::AddSphere(scene, 3, 3, 0, 1.25f, glm::vec3(0.0f, 0.0f, 1.0f));
	// SceneObjectFactory::AddTetrahedron(scene, 3, 0, 0, glm::vec3(1.0f, 1.0f, 1.0f));

	/* Lights. */
	SceneObjectFactory::Add2DQuad(scene, glm::vec2(1.5f, -1), glm::vec2(3.5f, 1), 3.76f,
								  glm::vec3(0, 0, -1), glm::vec3(1, 1, 1), glm::vec3(1.f, 1.f, 1.f));
	// SceneObjectFactory::AddEmissiveSphere(scene, -2, 0, 0, 0.5f, glm::vec3(1, 1, 1), glm::vec3(2, 2, 2));
	// SceneObjectFactory::AddEmissiveSphere(scene, 7, 2, 2, 0.5f, glm::vec3(1, 1, 1), glm::vec3(1.0f, 0.0f, 0.0f));

	scene.Initialize();

	/* Render. */
	c.Render(scene, 1024, glm::vec3(-7, 0, 0)); // Keep number of rays N so that you can write N as N = X^2 for some integer X.
	c.WriteImageToTGA();

	std::cout << "Rendering finished... press any key to exit." << std::endl;
	std::cin.get();

	return 0;
}

