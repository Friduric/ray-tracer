#pragma once

#include <vector>

#include <glm.hpp>

#include "../Geometry/Ray.h"
#include "../Rendering/RenderGroup.h"
#include "../Geometry/Triangle.h"

class Scene {
public:
	/// <summary> The groups in this scene which are renderable. </summary>
	std::vector<RenderGroup> renderGroups;
	std::vector<Material*> materials;

	Scene();
	~Scene();

	/// <summary> 
	/// Traces a ray through the scene and returns a color.
	/// </summary>
	glm::vec3 TraceRay(const Ray & ray, const unsigned int BOUNCES_PER_HIT = 20,
					   const unsigned int MAX_DEPTH = 5) const;

	/// <summary> 
	/// Casts a ray through the scene. Returns true if the was an intersection.
	/// </summary>
	/// <param name='ray'> The ray which we cast. </param>
	/// <param name='intersectionRenderGroupIndex'> 
	/// The intersection render group index if there was an intersection.
	/// </param>
	/// <param name='intersectionPrimitiveIndex'> 
	/// The intersection primitive index if there was an intersection.
	/// </param>
	/// <param name='intersectionPoint'> 
	/// The intersection point distance if there was an intersection.
	/// </param>
	bool Scene::RayCast(const Ray & ray,
						unsigned int & intersectionRenderGroupIndex,
						unsigned int & intersectionPrimitiveIndex,
						float & intersectionDistance) const;

	/// <summary> 
	/// Creates all walls, floors and ceilings of a room (according to the description given
	/// by Mark Eric Dieckmann in the course TNCG15) and adds them to the scene.
	/// (http://staffwww.itn.liu.se/~mardi/WebPages/Courses/TNCG15/courseTNCG15-2014)
	/// </summary>
	void CreateRoom();

	/// <summary> Creates a tetrahedron at position x, y, z and adds it to the scene. </summary>
	void CreateTetrahedron(float x = -35, float y = 0, float z = 0);

	/// <summary> Creates a sphere at position x, y, z and radius and adds it to the scene. </summary>
	void CreateSphere(float x = 3, float y = 0, float z = 0, float radius = 1.0f);
};