#pragma once

#include <iostream>
#include "Rendering\Camera.h"
#include "Utility\Math.h"

using namespace std;

///<summary> The main start for pengine. </summary>
int main()
{
	std::cout << "Starting..." << std::endl;

	// --- Interp test ---
	/*float t = Math::InterpolationQuad4f(0, 0, 0, 0, 1, 1);
	cout << t << endl;
	t = Math::InterpolationQuad4f(1, 1, 0, 0, 1, 1);
	cout << t << endl;*/
	// --- body ---
	Camera c(255, 255);
	Scene scene;
	scene.CreateRoom();
	// scene.CreateSphere();
	// scene.CreateTetrahedron();
	//			c1					c2					c3					c4
	c.Render(scene, glm::vec3(-2, 0, 0), glm::vec3(0, -5, 0), glm::vec3(0, 0, -3), glm::vec3(0, 7, 0), glm::vec3(0, 0, 10));
	c.WriteImageToTGA();
	// ------------

	std::cout << "Exiting... press any key to exit." << std::endl;
	std::cin.get();

	return 0;
}

