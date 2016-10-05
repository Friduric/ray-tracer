#include "Ray.h"

Ray::Ray(glm::vec4 _from, glm::vec4 _to, glm::vec3 _color) :
	from(_from), to(_to), color(_color), dir(glm::normalize(glm::vec3(_to) - glm::vec3(_from))) { }
