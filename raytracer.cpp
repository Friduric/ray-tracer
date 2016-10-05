#pragma once

#include <iostream>
#include "src\Graphics\Camera.h"

///<summary> The main start for pengine. </summary>
int main()
{
	std::cout << "Starting..." << std::endl;

	// --- body ---
	Camera c(1, 1);
	c.Render();
	c.WriteImageToTGA();
	// ------------

	std::cout << "Exiting... press any key to exit." << std::endl;
	std::cin.get();

	return 0;
}

