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
	bool RayCast(const Ray & ray, unsigned int & intersectionRenderGroupIndex, unsigned int & intersectionPrimitiveIndex, float & intersectionDistance) const;

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
	bool RenderGroupRayCast(const Ray & ray, unsigned int renderGroupIndex, unsigned int & intersectionPrimitiveIndex, float & intersectionDistance) const;
};