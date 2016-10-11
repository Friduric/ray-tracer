#pragma once

#include <glm.hpp>

class AABB {
public:
	glm::vec3 minimum, maximum;

	/// <summary> Constructs an AABB on the form (minimum, maximum). </summary>
	AABB(const glm::vec3 minimum, const glm::vec3 maximum);

	/// <summary> Constructs an AABB on the form ({minX, minY, minZ}, {maxX, maxY, maxZ}). </summary>
	AABB(const float minX, const float minY, const float minZ,
		 const float maxX, const float maxY, const float maxZ);

	/// <summary> Returns true if the given point is inside this AABB. </summary>
	bool IsPointInsideAABB(const glm::vec3 & point) const;
};