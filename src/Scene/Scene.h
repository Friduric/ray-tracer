#pragma once

#include <vector>
#include <functional>

#include <glm.hpp>

#include "../Geometry/Ray.h"
#include "../Rendering/RenderGroup.h"
#include "../Geometry/Triangle.h"
#include "../Geometry/AABB.h"

class Scene {
public:
	/// <summary> The groups in this scene which are renderable. </summary>
	std::vector<RenderGroup> renderGroups;
	std::vector<Material*> materials;
	std::vector<RenderGroup*> emissiveRenderGroups;

	/// <summary> Boundaries of the scene. </summary>
	AABB axisAlignedBoundingBox;

	/// <summary> Call this after all primitives has been added to the scene (pre-render). </summary>
	void Initialize();

	Scene();
	~Scene();

	/// <summary> 
	/// Traces a ray through the scene and returns a color.
	/// </summary>
	glm::vec3 TraceRay(const Ray & ray,
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
};