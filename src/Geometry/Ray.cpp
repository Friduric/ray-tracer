#include "Ray.h"

Ray::Ray(glm::vec3 _from, glm::vec3 _to) :
	from(_from),
	to(_to),
	dir(glm::normalize(glm::vec3(_to) - glm::vec3(_from))),
	length(glm::distance(_from, _to)) { }
