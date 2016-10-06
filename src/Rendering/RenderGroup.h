#pragma once

#include "Material\Material.h"
#include <vector>

class RenderGroup {
	Material* material;
	std::vector<Primitive> primitives;
	RenderGroup(Material* material);
};