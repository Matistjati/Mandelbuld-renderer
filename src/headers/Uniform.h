#pragma once
#ifndef UNIFORM_H
#define UNIFORM_H

#include <string>
#include <vector>
#include "nanogui/formhelper.h"

struct UniformSuper
{
public:
	std::vector<nanogui::Widget*> guiElements;
	std::function<void()> SetGuiValue;
	std::function<void(bool)> SetShaderValue;
	std::string name;
	unsigned int id;
	UniformSuper(std::vector<nanogui::Widget*> guiElements, std::function<void()> SetGuiValue, std::function<void(bool)> SetShaderValue, std::string name, unsigned int id)
		: guiElements(guiElements), name(name), id(id), SetGuiValue(SetGuiValue), SetShaderValue(SetShaderValue)
	{	}
};

template<typename T>
struct Uniform : public UniformSuper
{
	T value;
	T renderValue;
	T defaultValue;
	void operator=(const T &other);
	T& GetValue();
	void SetValue(T value, bool renderMode);

	Uniform(T val, T renderVal, std::string name, unsigned int id) : UniformSuper({}, {}, {}, name, id), value(val), renderValue(renderVal), defaultValue(val) {}
	Uniform(T val, std::string name, unsigned int id) :				 UniformSuper({}, {}, {}, name, id), value(val), renderValue(val), defaultValue(val) {}
	Uniform(T val, unsigned int id) :							     UniformSuper({}, {}, {}, "", id),   value(val), renderValue(val), defaultValue(val) {}
	Uniform(T val) :												 UniformSuper({}, {}, {}, "", -1),   value(val), renderValue(val), defaultValue(val) {}
	Uniform() :														 UniformSuper({}, {}, {}, "", -1),   value(T()), renderValue(T()), defaultValue(T()) {}

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