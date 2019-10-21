#pragma once
#ifndef UNIFORM_H
#define UNIFORM_H

#include <string>
#include <vector>
#include "nanogui/formhelper.h"


template<typename T>
struct Uniform
{
	std::vector<void*> guiElements;
	std::function<void()> SetGuiValue;
	std::function<void(bool)> SetShaderValue;
	std::string name;
	unsigned int id;
	T value;
	T renderValue;
	T defaultValue;
	void operator=(const T &other);
	T& GetValue();
	void SetValue(T value, bool renderMode);
#pragma warning(push)
#pragma warning(disable : 26495)
	Uniform(T val, T renderVal, std::string name, unsigned int id) : name(name), id(id), value(val), renderValue(renderVal), defaultValue(val) {}
	Uniform(T val, std::string name, unsigned int id) : name(name), id(id), value(val), renderValue(val), defaultValue(val) {}
	Uniform(T val, unsigned int id) : value(val), renderValue(val), defaultValue(value), id(id) {}
	Uniform(T val) : value(val), renderValue(val), defaultValue(val) {}
	Uniform() : value(T()), renderValue(T()), defaultValue(T()) {}
#pragma warning(pop)

	operator T() const;
};

// Don't even try to to include it up there; stuff will break left and right and you end up with 300+ errors
#include "headers/Fractal.h"

template<typename T>
inline T& Uniform<T>::GetValue()
{
	return (Fractal::renderMode) ? renderValue : value;
}

template<typename T>
inline void Uniform<T>::SetValue(T value, bool renderMode)
{
	if (!renderMode)
	{
		this->value = value;
	}
	this->renderValue = value;
}

#endif