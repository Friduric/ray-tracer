#pragma once

#include <vector>
#include "Triangle.h"

class Scene {
public:
	std::vector<Triangle> triangles;

	Scene();

	/// Creates all walls, floors and ceilings of a room (according to the description given
	/// by Mark Eric Dieckmann in the course TNCG15) and adds them to the scene.
	/// (http://staffwww.itn.liu.se/~mardi/WebPages/Courses/TNCG15/courseTNCG15-2014)
	void CreateRoom();

	/// Creates a tetrahedron at position x, y, z and adds it to the scene.
	void CreateTetrahedron(float x = 0, float y = 0, float z = 0);
};