#pragma once

#include <vector>
#include <functional>

#include <glm.hpp>

#include "../Geometry/Ray.h"
#include "../Rendering/RenderGroup.h"
#include "../Geometry/Triangle.h"
#include "../PhotonMap/PhotonMap.h"
#include "../Geometry/AABB.h"

class Scene {
public:
	/// <summary> The groups in this scene which are renderable. </summary>
	std::vector<RenderGroup> renderGroups;
	std::vector<Material*> materials;
	std::vector<RenderGroup*> emissiveRenderGroups;

	/// <summary> Boundaries of the scene. </summary>
	AABB axisAlignedBoundingBox;

	/// <summary> Photon Map. </summary>
	PhotonMap* photonMap = nullptr;

	/// <summary> Call this after all primitives has been added to the scene (pre-render). </summary>
	void Initialize();

	Scene();
	~Scene();

	/// <summary> 
	/// Casts rays towards all lightsources from the intersectionPoint pos at
	/// the Primitive prim and calculate the direct illumination at the 
	/// Primitive prim at with Material material.
	/// </summary>
	glm::vec3 Scene::CalculateDirectIlluminationAtPos(const glm::vec3 & pos, const glm::vec3 & incomingDirection, const Primitive & prim, const Material & material) const;

	/// <summary> 
	/// Traces a ray through the scene and returns a color.
	/// </summary>
	glm::vec3 TraceRay(const Ray & ray,
					   const unsigned int BOUNCES_PER_HIT = 1,
					   const unsigned int MAX_DEPTH = 5) const;

	/// <summary> 
	/// Traces a ray through the scene and returns a color, using photon map.
	/// </summary>
	glm::vec3 TraceRayUsingPhotonMap(const Ray & ray,
									 const unsigned int BOUNCES_PER_HIT = 1,
									 const unsigned int MAX_DEPTH = 5) const;

	/// <summary> 
	/// Casts a ray through the scene. Returns true if there was an intersection.
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
	bool RayCast(const Ray & ray,
				 unsigned int & intersectionRenderGroupIndex,
				 unsigned int & intersectionPrimitiveIndex,
				 float & intersectionDistance, bool cullBackFace = true) const;

	/// <summary> 
	/// Casts a refractive ray through the scene. Returns true if there was an intersection.
	/// </summary>
	/// <param name='ray'> The ray which hit a surface which we are now tunneling through. </param>
	/// <param name='intersectionRenderGroupIndex'> 
	/// The intersection render group index which we intersected. </param>
	/// <param name='normal'> The normal to the surface which we intersected. </param>
	/// <param name='intersectionPoint'> The point which the ray intersected at. </param>
	/// <param name='materialFrom'> The material which we are coming from. </param>
	/// <param name='materialTo'> The material which we are leaving. </param>
	bool RefractionRayCast(const Ray & ray, const unsigned int intersectionRenderGroupIndex,
						   const glm::vec3 & normal,
						   const glm::vec3 & intersectionPoint,
						   const Material * const materialFrom,
						   const Material * const materialTo) const;

	/// <summary> 
	/// Generates a photon map that can be used during rendering.
	/// </summary>
	/// <param name='scene'> The scene. </param>
	/// <param name='PHOTONS_PER_LIGHT_SOURCE'> The amount of photons used per light source. </param>
	/// <param name='MAX_PHOTONS_PER_NODE'> The maximum amount of photons per node. </param>
	/// <param name='MAXIMUM_NODE_BOX_DIMENSION'> The maximum width, height and depth of the node box size. </param>
	/// <param name='MAX_DEPTH'> The amount of times each photon will bounce at most. </param>
	void GeneratePhotonMap(const unsigned int PHOTONS_PER_LIGHT_SOURCE = 200000,
						   const unsigned int MAX_PHOTONS_PER_NODE = 100,
						   const unsigned int MAX_DEPTH = 5);
};