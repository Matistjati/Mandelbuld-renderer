#pragma once
#ifndef BufferInitialization_H
#define BufferInitialization_H

#include <map>
#include <string>
#include <glm.hpp>
#include <vector>

typedef void (*InitFunction)(std::vector<glm::vec4>&, glm::ivec2, std::vector<float>);

class BufferInitialization
{
public:
	static std::map<std::string, InitFunction> functions;
};

#endif // !BufferInitialization_H



