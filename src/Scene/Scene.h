#pragma once

#include <vector>
#include <functional>

#include <glm.hpp>

#include "../Geometry/Ray.h"
#include "../Rendering/RenderGroup.h"
#include "../Geometry/Triangle.h"

class Scene {
public:
	/// <summary> The groups in this scene which are renderable. </summary>
	std::vector<RenderGroup> renderGroups;
	std::vector<Material*> materials;
	std::vector<RenderGroup*> emissiveRenderGroups;

	/// <summary> Boundaries of the scene. </summary>
	float xMin, yMin, zMin = FLT_MAX*0.5f;
	float xMax, yMax, zMax = -FLT_MAX*0.5f;

	/// <summary> Call this after all primitives has been added to the scene (pre-render). </summary>
	void Initialize();

	Scene();
	~Scene();

	/// <summary> 
	/// Traces a ray through the scene and returns a color.
	/// </summary>
	glm::vec3 TraceRay(Ray & ray,
					   const unsigned int BOUNCES_PER_HIT = 1,
					   const unsigned int MAX_DEPTH = 5) const;

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
										  const unsigned int numberOfRays) const;
};