#pragma once

#include <iostream>
#include "Rendering\Camera.h"
#include "Utility\Math.h"

using namespace std;

///<summary> The main start for pengine. </summary>
int main()
{
	std::cout << "Starting..." << std::endl;

	/* Initialize camera. */
	Camera c(90, 90);

	/* Create scene. */
	Scene scene;
	scene.CreateRoom();
	scene.CreateSphere(10, 0, 2, 1.0f);
	scene.CreateTetrahedron(3, 0, 0);

	/* Render. */
	c.Render(scene);
	c.WriteImageToTGA();

	std::cout << "Exiting... press any key to exit." << std::endl;
	std::cin.get();

	return 0;
}

