#pragma once
#ifndef TOSTRING_H
#define TOSTRING_H

#include "glm.hpp"
#include <string>

std::string toString(glm::dvec2 vector)
{
	return "x: " + std::to_string(vector.x) + ", y: " + std::to_string(vector.y);
}

std::string toString(glm::dvec3 vector)
{
	return "x: " + std::to_string(vector.x) + ", y: " + std::to_string(vector.y) + ", z: " + std::to_string(vector.z);
}

#endif
