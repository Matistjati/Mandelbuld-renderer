#pragma once
#ifndef UNIFORM_H
#define UNIFORM_H

#include <string>
#include <vector>
#include "nanogui/formhelper.h"

class Shader;

struct UniformSuper
{
public:
	std::vector<nanogui::Widget*> guiElements;
	std::function<void()> SetGuiValue;
	std::function<void(bool)> SetShaderValue;
	std::string name;
	unsigned int id;
	unsigned int programId;
	
	bool objectMember;

	// Callback related, fractal has to be null pointer due to c++ not being able to handle fractal and uniform using each other
	void* fractal;
	std::vector<std::function<void(void*)>> callbacks;

	UniformSuper(std::vector<nanogui::Widget*> guiElements, std::function<void()> SetGuiValue, std::function<void(bool)> SetShaderValue, std::string name, unsigned int id, unsigned int programId, bool objectMember = true)
		: guiElements(guiElements), name(name), id(id), SetGuiValue(SetGuiValue), SetShaderValue(SetShaderValue), programId(programId), callbacks({}), fractal(), objectMember(objectMember)
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
	void Reset() { value = defaultValue; renderValue = defaultValue; }

	Uniform(T val, T renderVal, std::string name, unsigned int id, unsigned int programId, bool objectMember) : UniformSuper({}, {}, {}, name, id, programId, objectMember), value(val), renderValue(renderVal), defaultValue(val) {}
	Uniform(T val, T renderVal, std::string name, unsigned int id) : UniformSuper({}, {}, {}, name, id, -1), value(val), renderValue(renderVal), defaultValue(val) {}
	Uniform(T val, std::string name, unsigned int id) :				 UniformSuper({}, {}, {}, name, id, -1), value(val), renderValue(val), defaultValue(val) {}
	Uniform(T val, unsigned int id) :							     UniformSuper({}, {}, {}, "", id, -1),   value(val), renderValue(val), defaultValue(val) {}
	Uniform(T val) :												 UniformSuper({}, {}, {}, "", -1, -1),   value(val), renderValue(val), defaultValue(val) {}
	Uniform() :														 UniformSuper({}, {}, {}, "", -1, -1),   value(T()), renderValue(T()), defaultValue(T()) {}

	operator T() const;
};

// Don't even try to to include it up there; stuff will break left and right and you end up with 300+ errors
#include "headers/Fractal.h"

class Fractal;

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
	for (size_t i = 0; i < callbacks.size(); i++)
	{
		callbacks[i](fractal);
	}
}

#endif