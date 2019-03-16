#pragma once
#ifndef TOSTRING_H
#define TOSTRING_H

#include "glm.hpp"
#include <string>

class ToString
{
public:
	static std::string toString(glm::vec2 vector);
	static std::string toString(glm::vec3 vector);
private:
	ToString() { };
	~ToString() { };
};

#endif
