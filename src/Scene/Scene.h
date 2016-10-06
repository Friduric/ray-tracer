#pragma once

#include <vector>

#include <glm.hpp>

#include "../Geometry/Triangle.h"
#include "../Geometry/Ray.h"

class Scene {
public:
	std::vector<Triangle> triangles;

	Scene();

	/// <summary> 
	/// Traces a ray through the scene and returns a color.
	/// </summary>
	glm::vec3 TraceRay(const Ray & ray);

	/// <summary> 
	/// Creates all walls, floors and ceilings of a room (according to the description given
	/// by Mark Eric Dieckmann in the course TNCG15) and adds them to the scene.
	/// (http://staffwww.itn.liu.se/~mardi/WebPages/Courses/TNCG15/courseTNCG15-2014)
	/// </summary>
	void CreateRoom();

	/// <summary> Creates a tetrahedron at position x, y, z and adds it to the scene. </summary>
	void CreateTetrahedron(float x = 0, float y = 0, float z = 0);
};