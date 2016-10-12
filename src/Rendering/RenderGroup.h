#pragma once

#include <vector>
#include <memory>

#include "Materials\Material.h"
#include "..\Geometry\Primitive.h"
#include "..\PhotonMap\Photon.h"

class RenderGroup {
public:
	Material* material;
	std::vector<Primitive*> primitives;
	std::vector<std::vector<Photon>> photons;
	glm::vec3 GetRandomPositionOnSurface();
	RenderGroup(Material*);
};