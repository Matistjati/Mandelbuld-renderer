#pragma once
#ifndef UNIFORM_H
#define UNIFORM_H

#include <string>
#include "nanogui/formhelper.h"

template<typename T>
struct Uniform
{
	void* guiElement;
	std::function<void()> SetGuiValue;
	std::string name;
	unsigned int id;
	T value;
	void operator=(const T &other);
#pragma warning(push)
#pragma warning(disable : 26495)
	Uniform(T val, std::string name, unsigned int id) : name(name), id(id), value(val) {}
	Uniform(T val, unsigned int id) : value(val), id(id) {}
	Uniform(T val) : value(val) {}
	Uniform() : value(T()) {}
#pragma warning(pop)

	operator T() const;
};

#endif