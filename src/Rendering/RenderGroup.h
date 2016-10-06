#pragma once

#include "Material\Material.h"
#include <vector>
#include "..\Geometry\Primitive.h"

class RenderGroup {
	Material* material;
	std::vector<Primitive> primitives;
	RenderGroup(Material* material);
};