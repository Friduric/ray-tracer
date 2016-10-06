#pragma once

#include "Material\Material.h"
#include <vector>
#include "..\Geometry\Primitive.h"
#include "Photon.h"

class RenderGroup {
public:
	Material* material;
	std::vector<Primitive> primitives;
	std::vector<std::vector<Photon>> photons;
	RenderGroup(Material* material);
};