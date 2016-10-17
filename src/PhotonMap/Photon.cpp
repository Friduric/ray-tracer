#include "Photon.h"

Photon::Photon() {}

Photon::Photon(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _color, glm::vec3 _normal) :
	position(_position), direction(_direction), color(_color), normal(_normal) {}
