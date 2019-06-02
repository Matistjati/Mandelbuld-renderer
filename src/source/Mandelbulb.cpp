#include <glew.h>
#include <windows.h>
#include "headers/Mandelbulb.h"
#include "glm.hpp"
#include "headers/shader.h"
#include "headers/camera.h"
#include <iostream>
#include "headers/Image.h"
#include "headers/Debug.h"
#include <string>
#include <fstream>

Mandelbulb::Mandelbulb(float power, Camera &camera, glm::vec3 sun, glm::ivec2 screenSize, Time time, int specIndex)
	: Fractal3D(GenerateShader(false, specIndex), GenerateShader(true, specIndex), camera, screenSize, time, sun), power(power), genericParameter(1)
{
	SetVariablesFromSpec(specIndex);
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbulb::Mandelbulb(float power, Camera &camera, glm::vec3 sun, int specIndex)
	: Fractal3D(GenerateShader(false, specIndex), GenerateShader(true, specIndex), camera, glm::ivec2(Fractal::DefaultWidth, Fractal::DefaultHeight), Time(), sun), power(power), genericParameter(1)
{
	SetVariablesFromSpec(specIndex);
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbulb::Mandelbulb(float power, Camera &camera, int specIndex)
	: Fractal3D(GenerateShader(false, specIndex), GenerateShader(true, specIndex), camera, glm::ivec2(Fractal::DefaultWidth, Fractal::DefaultHeight), Time(), glm::normalize(glm::dvec3(0.577, 0.577, 0.577))), power(power), genericParameter(1)
{
	SetVariablesFromSpec(specIndex);
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbulb::Mandelbulb(int specIndex)
	: Fractal3D(GenerateShader(false, specIndex), GenerateShader(true, specIndex)), power(defaultPower), genericParameter(1)
{
	SetVariablesFromSpec(specIndex);
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

void Mandelbulb::KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	Fractal3D::KeyCallback(window, key, scancode, action, mods);

	switch (key)
	{
		// Changing the power of the fractal
	case GLFW_KEY_C:
		power.value += 0.03f * parameterChangeRate;
		explorationShader.SetUniform(power);
		break;
	case GLFW_KEY_V:
		power.value -= 0.03f * parameterChangeRate;
		explorationShader.SetUniform(power);
		break;

		// Debugging
	case GLFW_KEY_R:
		genericParameter.value += 0.1f * parameterChangeRate;
		explorationShader.SetUniform(genericParameter);
		break;
	case GLFW_KEY_F:
		genericParameter.value -= 0.1f * parameterChangeRate;
		explorationShader.SetUniform(genericParameter);
		break;
		

	default:
		break;
	}
}

void Mandelbulb::SetUniforms(Shader& shader)
{
	GlErrorCheck();
	shader.use();

	shader.SetUniform(power);
	shader.SetUniform(genericParameter);
	Fractal3D::SetUniforms(shader);
}

void Mandelbulb::SetUniformLocations(Shader &shader)
{
	GlErrorCheck();
	power.id = glGetUniformLocation(shader.id, power.name.c_str());
	genericParameter.id = glGetUniformLocation(shader.id, genericParameter.name.c_str());
	Fractal3D::SetUniformLocations(shader);
}

void Mandelbulb::SetUniformNames()
{
	power.name = "power";
	genericParameter.name = "genericParameter";
	Fractal3D::SetUniformNames();
}

void Mandelbulb::Update()
{
	time.PollTime();


	// Move sun in shader
	double time = glfwGetTime();
	sun.value = glm::normalize(glm::vec3(sin(time * 0.25),
		std::abs(sin(time * 0.1)) * -1,
		cos(time * 0.25)));

	genericParameter.value = (float)(abs(cos(time * 0.025) * 0.5 - sin(time * 0.01) * 2));
	explorationShader.SetUniform(sun);
	explorationShader.SetUniform(genericParameter);
}


Shader& Mandelbulb::GenerateShader(bool highQuality, int specIndex)
{
	GlErrorCheck();

	std::string source = readFile(SourcePath);

	std::string base = readFile(Fractal3D::path3DBase);

	std::string specification = readFile(SpecificationPath);

	const size_t length = std::extent<decltype(mandelBulbShaderSections)>::value;
	Fractal3D::ParseShader(source, base, specification, highQuality, specIndex, mandelBulbShaderSections, length);

	std::string vertexSource = readFile(Fractal::pathRectangleVertexshader);

	return *(new Shader(vertexSource, base, false));
}

void Mandelbulb::SetVariablesFromSpec(int index)
{
	std::string specSection = GetSpecificationByIndex(readFile(SpecificationPath), index);
	std::string variables = getSection(Section("cpuVariables"), specSection);
	if (variables != "")
	{
		std::vector<std::string> variablesList = splitNotInChar(variables, ',', '[', ']');
		for (size_t i = 0; i < variablesList.size(); i++)
		{
			std::string value = getSectionValue(variablesList[i]);
			cleanString(value, { '[', ']' });
			SetVariable(getSectionName(variablesList[i]), value);
		}
	}
}

void Mandelbulb::SetVariable(std::string name, std::string value)
{
	if (name == "power")
	{
		power.value = std::stof(value);
	}
	else if (name == "genericParameter")
	{
		genericParameter.value = std::stof(value);
	}
	else
	{
		Fractal3D::SetVariable(name, value);
	}
}