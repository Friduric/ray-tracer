#include "Photon.h"

Photon::Photon() {}

Photon::Photon(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _color, Primitive* _prim) :
	position(_position), direction(_direction), color(_color), prim(_prim) {}
