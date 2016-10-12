#pragma once

#include "Scene.h"

namespace SceneObjectFactory {
	/// <summary> 
	/// Creates all walls, floors and ceilings of a room (according to the description given
	/// by Mark Eric Dieckmann in the course TNCG15) and adds them to the scene.
	/// (http://staffwww.itn.liu.se/~mardi/WebPages/Courses/TNCG15/courseTNCG15-2014)
	/// </summary>
	void AddRoom(Scene & scene, bool addBackWalls = false, bool emissiveCeiling = false);

	/// <summary> Creates a tetrahedron at position x, y, z and adds it to the scene. </summary>
	void AddTetrahedron(Scene & scene, float x = 0, float y = 0, float z = 0,
						glm::vec3 surfaceColor = glm::vec3(0, 0, 1));

	/// <summary> Creates a sphere at position x, y, z and radius and adds it to the scene. </summary>
	void AddSphere(Scene & scene, float x = 0, float y = 0, float z = 0, float radius = 0,
				   glm::vec3 surfaceColor = glm::vec3(1, 0, 0));

	/// <summary> Creates a sphere at position x, y, z and radius and adds it to the scene. </summary>
	void AddEmissiveSphere(Scene & scene, float x = 0, float y = 0, float z = 0,
						   float radius = 1.0f, glm::vec3 surfaceColor = glm::vec3(1, 1, 1),
						   float emissivity = 1.0f);

	/// <summary> Creates a sphere at position x, y, z and radius and adds it to the scene. </summary>
	void AddOrenNayarSphere(Scene & scene, float x = 0, float y = 0, float z = 0, float radius = 0,
							glm::vec3 surfaceColor = glm::vec3(1, 0, 0));

	/// <summary> Creates a triangle and adds it to the scene. </summary>
	void AddTriangle(Scene & scene, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal,
					 glm::vec3 surfaceColor, float emissivity = 1.0f);

	/// <summary> Creates a quad and adds it to the scene. </summary>
	void Add2DQuad(Scene & scene, glm::vec2 corner1, glm::vec2 corner2, float height,
				   glm::vec3 normal = glm::vec3(0, 0, -1),
				   glm::vec3 surfaceColor = glm::vec3(1, 1, 1),
				   float emissivity = 1.0f);
};