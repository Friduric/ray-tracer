#pragma once
#pragma once

#include "Renderer.h"
#include "../../Scene/Scene.h"

class PhotonMapVisualizer : public Renderer {
public:
	glm::vec3 GetPixelColor(const Ray & ray) override;
	PhotonMapVisualizer(const Scene & scene);
private:
	glm::vec3 TraceRay(const Ray & ray, const unsigned int MAX_DEPTH);
	const Scene & SCENE;
};