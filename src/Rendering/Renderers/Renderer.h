#pragma once

#include <glm.hpp>
#include "../../Geometry/Ray.h"

class Renderer {
public:
	virtual glm::vec3 GetPixelColor(const Ray & ray) = 0;
	const std::string RENDERER_NAME = "Unknown Name";
protected:
	Renderer(const std::string NAME) : RENDERER_NAME(NAME) { }
};