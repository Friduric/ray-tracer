#include "RenderGroup.h"

glm::vec3 RenderGroup::GetRandomPositionOnSurface() {
	const auto & primitive = primitives[rand() % primitives.size()];
	return primitive->GetRandomPositionOnSurface();
}

RenderGroup::RenderGroup(Material * mat) : material(mat) {}
