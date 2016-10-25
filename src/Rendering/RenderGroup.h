#pragma once

#include <vector>
#include <memory>

#include "Materials\Material.h"
#include "..\Geometry\Primitive.h"
#include "..\PhotonMap\Photon.h"
#include "..\Geometry\AABB.h"

class RenderGroup {
public:
	bool enabled = true;
	bool convex = true;
	AABB axisAlignedBoundingBox;
	Material* material;
	std::vector<Primitive*> primitives;
	std::vector<std::vector<Photon>> photons;

	RenderGroup(Material*);
	void RecalculateAABB();
	glm::vec3 GetRandomPositionOnSurface() const;
};