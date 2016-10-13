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

	/// <summary> Returns a primitive given it's render group index and primitive index. </summary>
	Primitive & GetPrimitive(unsigned int renderGroupIndex, unsigned int primitiveIndex);

	void RecalculateAABB();

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
	/// Casts a ray through a given render group. Returns true if there was an intersection.
	/// </summary>
	/// <param name='ray'> The ray which we cast. </param>
	/// <param name='renderGroupIndex'> 
	/// The intersection render group index which we ray cast at.
	/// All other render groups are ignored!
	/// </param>
	/// <param name='intersectionPrimitiveIndex'> 
	/// OUT: The intersection primitive index if there was an intersection.
	/// </param>
	/// <param name='intersectionPoint'> 
	/// OUT: The intersection point distance if there was an intersection.
	/// </param>
	bool RenderGroupRayCast(const Ray & ray, unsigned int renderGroupIndex,
							unsigned int & intersectionPrimitiveIndex,
							float & intersectionDistance, bool cullBackFace = false) const;

	/// <summary> 
	/// Casts a refractive ray through the scene. Returns true if there was an intersection.
	/// </summary>
	/// <param name='ray'> The ray which hit a surface which we are now tunneling through. </param>
	/// <param name='intersectionRenderGroupIndex'> 
	/// The intersection render group index which we intersected. </param>
	/// <param name='normal'> The normal to the surface which we intersected. </param>
	/// <param name='intersectionPoint'> The point which the ray intersected at. </param>
	/// <param name='materialFrom'> 
	/// The material which we are coming from. If nullptr the material is considered to be air. 
	/// </param>
	/// <param name='materialTo'> 
	/// The material which we are leaving. If nullptr the material is considered to be air.
	/// </param>
	bool RefractionRayCast(const Ray & incomingRay, const unsigned int intersectionRenderGroupIndex,
						   const glm::vec3 & intersectionPointNormal,
						   const glm::vec3 & intersectionPoint,
						   const Material * const materialFrom = nullptr,
						   const Material * const materialTo = nullptr) const;

};