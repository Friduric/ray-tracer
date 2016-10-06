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
	Camera c(100, 100);
	Scene scene;					//			c1					c2					c3					c4
	c.Render(scene, glm::vec3(-2, 0, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
	c.WriteImageToTGA();
	// ------------

	std::cout << "Exiting... press any key to exit." << std::endl;
	std::cin.get();

	return 0;
}

