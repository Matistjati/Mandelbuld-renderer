#include "headers/ToString.h"

std::string ToString::toString(glm::dvec2 vector)
{
	return "x: " + std::to_string(vector.x) + ", y: " + std::to_string(vector.y);
}

std::string ToString::toString(glm::dvec3 vector)
{
	return "x: " + std::to_string(vector.x) + ", y: " + std::to_string(vector.y) + ", z: " + std::to_string(vector.z);
}
