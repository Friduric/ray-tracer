#pragma once

#include <vector>

#include <glm.hpp>

#include "../Geometry/Ray.h"
#include "../Rendering/RenderGroup.h"
#include "../Geometry/Triangle.h"
#include <functional>

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
	glm::vec3 TraceRay(const Ray & ray,
					   const unsigned int BOUNCES_PER_HIT = 20,
					   const unsigned int MAX_DEPTH = 5,
					   const float DEPRECATION_FACTOR = 1.0f) const;

	/// <summary> 
	/// Casts a ray through the scene. Returns true if the was an intersection.
	/// </summary>
	/// <param name='ray'> The ray which we cast. </param>
	/// <param name='intersectionRenderGroupIndex'> 
	/// OUT: The intersection render group index if there was an intersection.
	/// </param>
	/// <param name='intersectionPrimitiveIndex'> 
	/// OUT: The intersection primitive index if there was an intersection.
	/// </param>
	/// <param name='intersectionPoint'> 
	/// OUT: The intersection point distance if there was an intersection.
	/// </param>
	bool Scene::RayCast(const Ray & ray,
						unsigned int & intersectionRenderGroupIndex,
						unsigned int & intersectionPrimitiveIndex,
						float & intersectionDistance) const;

	/// <summary>
	///	Generates "bouncing" rays given a surface and it's properties.
	/// </summary>
	/// <param name='incomingDirection'> The direction of the incoming ray. </param>
	/// <param name='hitNormal'> The normal of the surface for which we are bouncing on. </param>
	/// <param name='intersectionPoint'> The origin of the generated rays </param>
	/// <param name='material'> The material of the surface for which we are bouncing on. </param>
	/// <param name='numberOfRays'> The number of rays to be generated. </param>
	/// <param name='rayLength'> The length of the generated rays. </param>
	/// <remarks> 
	/// Parameter 'material' cannot be const since we re-use random generators from within the 
	/// material.
	/// </remarks>
	std::vector<Ray> GenerateBouncingRays(const glm::vec3 & incomingDirection,
										  const glm::vec3 & hitNormal,
										  const glm::vec3 & intersectionPoint,
										  Material* material,
										  const unsigned int numberOfRays,
										  const float rayLength) const;

	/// <summary> 
	/// Creates all walls, floors and ceilings of a room (according to the description given
	/// by Mark Eric Dieckmann in the course TNCG15) and adds them to the scene.
	/// (http://staffwww.itn.liu.se/~mardi/WebPages/Courses/TNCG15/courseTNCG15-2014)
	/// </summary>
	void CreateRoom();

	/// <summary> Creates a tetrahedron at position x, y, z and adds it to the scene. </summary>
	void CreateTetrahedron(float x = 0, float y = 0, float z = 0);

	/// <summary> Creates a sphere at position x, y, z and radius and adds it to the scene. </summary>
	void CreateSphere(float x = 0, float y = 0, float z = 0, float radius = 1.0f);

	/// <summary> Creates a sphere at position x, y, z and radius and adds it to the scene. </summary>
	void CreateEmissiveSphere(float x = 0, float y = 0, float z = 0,
							  float radius = 1.0f, glm::vec3 emissionColor = glm::vec3(1, 1, 1));
};