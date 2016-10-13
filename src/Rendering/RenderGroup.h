#pragma once

#include <vector>
#include <memory>

#include "Materials\Material.h"
#include "..\Geometry\Primitive.h"
#include "..\PhotonMap\Photon.h"

class RenderGroup {
public:
	bool enabled = true;
	bool convex = true;
	Material* material;
	std::vector<Primitive*> primitives;
	std::vector<std::vector<Photon>> photons;
	RenderGroup(Material*);
	glm::vec3 GetRandomPositionOnSurface();
};