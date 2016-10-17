#include "Photon.h"

Photon::Photon() {
	position = glm::vec3(0, 0, 0);
	direction = glm::vec3(0, 0, 0);
	color = glm::vec3(0, 0, 0);
}

Photon::Photon(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _color) :
	position(_position), direction(_direction), color(_color) {}
