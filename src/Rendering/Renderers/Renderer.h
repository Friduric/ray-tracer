#pragma once

#include <string>

#include <glm.hpp>

#include "../../Geometry/Ray.h"
#include "../Materials/Material.h"
#include "../../Scene/Scene.h"

class Renderer {
public:
	virtual glm::vec3 GetPixelColor(const Ray & ray) = 0;
	const std::string RENDERER_NAME = "Unknown Name";
protected:
	Renderer(const std::string NAME, Scene & _scene) : RENDERER_NAME(NAME), scene(_scene) { }
	Scene & scene;
};