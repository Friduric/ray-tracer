#include "RenderGroup.h"

glm::vec3 RenderGroup::GetRandomPositionOnSurface() {
	const auto & primitive = primitives[rand() % primitives.size()];
	return primitive->GetRandomPositionOnSurface();
}

RenderGroup::RenderGroup(Material * mat) : material(mat) {}

void RenderGroup::RecalculateAABB() {
	glm::vec3 minimum = glm::vec3(FLT_MAX);
	glm::vec3 maximum = glm::vec3(-FLT_MAX);
	for (const auto & p : primitives) {
		const auto & aabb = p->GetAxisAlignedBoundingBox();
		const auto & amin = aabb.minimum;
		const auto & amax = aabb.maximum;
		minimum.x = glm::min<float>(minimum.x, amin.x);
		minimum.y = glm::min<float>(minimum.y, amin.y);
		minimum.z = glm::min<float>(minimum.z, amin.z);
		maximum.x = glm::max<float>(maximum.x, amax.x);
		maximum.y = glm::max<float>(maximum.y, amax.y);
		maximum.z = glm::max<float>(maximum.z, amax.z);
	}
	axisAlignedBoundingBox.minimum = minimum;
	axisAlignedBoundingBox.maximum = maximum;
}
