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
	std::function<void()> PrintValue;
	std::function<void(bool)> SetShaderValue;
	std::string name;
	unsigned int id;
	unsigned int programId;
	
	bool objectMember;

	// Callback related, fractal has to be null pointer due to c++ not being able to handle fractal and uniform using each other
	void* fractal;
	std::vector<std::function<void(void*)>> callbacks;

	UniformSuper(std::vector<nanogui::Widget*> guiElements, std::function<void()> SetGuiValue, std::function<void(bool)> SetShaderValue, std::function<void()> PrintValue, std::string name, unsigned int id, unsigned int programId, bool objectMember = true)
		: guiElements(guiElements), name(name), id(id), SetGuiValue(SetGuiValue), SetShaderValue(SetShaderValue), programId(programId), callbacks({}), fractal(), objectMember(objectMember), PrintValue(PrintValue)
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
	void PrintValueConsole();

	Uniform(T val, T renderVal, std::string name, unsigned int id, unsigned int programId, bool objectMember) : UniformSuper({}, {}, {}, [this]() {this->PrintValueConsole(); }, name, id, programId, objectMember), value(val), renderValue(renderVal), defaultValue(val) {}
	Uniform(T val, T renderVal, std::string name, unsigned int id) : UniformSuper({}, {}, {}, [this]() {this->PrintValueConsole(); }, name, id, -1), value(val), renderValue(renderVal), defaultValue(val) {}
	Uniform(T val, std::string name, unsigned int id) :				 UniformSuper({}, {}, {}, [this]() {this->PrintValueConsole(); }, name, id, -1), value(val), renderValue(val), defaultValue(val) {}
	Uniform(T val, unsigned int id) :								 UniformSuper({}, {}, {}, [this]() {this->PrintValueConsole(); }, "", id, -1), value(val), renderValue(val), defaultValue(val) {}
	Uniform(T val) :												 UniformSuper({}, {}, {}, [this]() {this->PrintValueConsole(); }, "", -1, -1), value(val), renderValue(val), defaultValue(val) {}
	Uniform() :														 UniformSuper({}, {}, {}, [this]() {this->PrintValueConsole(); }, "", -1, -1), value(T()), renderValue(T()), defaultValue(T()) {}

	operator T() const;
};

// Don't even try to to include it up there; stuff will break left and right and you end up with 300+ errors
#include "headers/Fractal.h"
#include "nanogui/nanogui.h"
#include "headers/Time.h"
#include "glm.hpp"

class Fractal;
class nanogui::Color;

template<typename T>
inline T& Uniform<T>::GetValue()
{
	return (Fractal::renderMode) ? renderValue : value;
}

template<typename T>
inline void Uniform<T>::SetValue(T value, bool renderMode)
{
	bool different = value != this->renderValue;
	if (!renderMode)
	{
		this->value = value;
	}
	this->renderValue = value;
	if (different)
	{
		for (size_t i = 0; i < callbacks.size(); i++)
		{
			callbacks[i](fractal);
		}
	}
}

template<typename T>
inline void Uniform<T>::PrintValueConsole()
{
	std::cout << name << ": " << GetValue() << std::endl;
}

inline void Uniform<bool>::PrintValueConsole()
{
	std::cout << name << ": " << (GetValue() ? "true" : "false") << std::endl;
}

inline void Uniform<int>::PrintValueConsole()
{
	std::cout << name << ": " << GetValue() << std::endl;
}

inline void Uniform<unsigned int>::PrintValueConsole()
{
	std::cout << name << ": " << GetValue() << std::endl;
}

inline void Uniform<Time>::PrintValueConsole()
{
	std::cout << name << ": {Total time: " << GetValue().GetTotalTime() << ", delta time: " << GetValue().GetDeltaTime() << "}" << std::endl;
}

inline void Uniform<glm::vec2>::PrintValueConsole()
{
	std::cout << name << ": {" << GetValue().x << ", " << GetValue().y << "}" << std::endl;
}

inline void Uniform<glm::vec3>::PrintValueConsole()
{
	std::cout << name << ": {" << GetValue().x << ", " << GetValue().y << ", " << GetValue().z << "}" << std::endl;
}

inline void Uniform<glm::vec4>::PrintValueConsole()
{
	std::cout << name << ": {" << GetValue().x << ", " << GetValue().y << ", " << GetValue().z << "," << GetValue().w << "}" << std::endl;
}

inline void Uniform<glm::mat3>::PrintValueConsole()
{
	std::cout << name << ": {" << GetValue()[0][0] << ", " << GetValue()[0][1] << ", " << GetValue()[0][2] << ", "
							   << GetValue()[1][0] << ", " << GetValue()[1][1] << ", " << GetValue()[1][2] << ", "
							   << GetValue()[2][0] << ", " << GetValue()[2][1] << ", " << GetValue()[2][2] << "}" << std::endl;
}

inline void Uniform<nanogui::Color>::PrintValueConsole()
{
	std::cout << name << ": {" << GetValue().x() << ", " << GetValue().y() << ", " << GetValue().z() << "}" << std::endl;
}

#endif