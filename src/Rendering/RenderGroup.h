#pragma once

#include "Material\Material.h"
#include <vector>
#include "..\Geometry\Primitive.h"

class RenderGroup {
public:
	Material* material;
	std::vector<Primitive> primitives;
	RenderGroup(Material* material);
};