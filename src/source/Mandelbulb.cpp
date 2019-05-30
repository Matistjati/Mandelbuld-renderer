#include <glew.h>
#include <windows.h>
#include "headers/Mandelbulb.h"
#include "glm.hpp"
#include "headers/shader.h"
#include "headers/camera.h"
#include <iostream>
#include "headers/Image.h"
#include "headers/GlError.h"
#include <string>
#include <fstream>

Mandelbulb::Mandelbulb(float power, Camera &camera, glm::vec3 sun, glm::ivec2 screenSize, Time time)
	: Fractal3D(ParseShader(false), ParseShader(true), camera, screenSize, time, sun), power(power), genericParameter(1)
{
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbulb::Mandelbulb(float power, Camera &camera, glm::vec3 sun)
	: Fractal3D(ParseShader(false), ParseShader(true), camera, glm::ivec2(Fractal::DefaultWidth, Fractal::DefaultHeight), Time(), sun), power(power), genericParameter(1)
{
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbulb::Mandelbulb(float power, Camera &camera)
	: Fractal3D(ParseShader(false), ParseShader(true), camera, glm::ivec2(Fractal::DefaultWidth, Fractal::DefaultHeight), Time(), glm::normalize(glm::dvec3(0.577, 0.577, 0.577))), power(power), genericParameter(1)
{
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbulb::Mandelbulb()
	: Fractal3D(ParseShader(false), ParseShader(true)), power(defaultPower), genericParameter(1)
{
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
		power.value += 0.03f;
		explorationShader.SetUniform(power);
		break;
	case GLFW_KEY_V:
		power.value -= 0.03f;
		explorationShader.SetUniform(power);
		break;

		// Debugging
	case GLFW_KEY_R:
		genericParameter.value += 0.1f;
		explorationShader.SetUniform(genericParameter);
		break;
	case GLFW_KEY_F:
		genericParameter.value -= 0.05f;
		explorationShader.SetUniform(genericParameter);
		break;
		

	default:
		break;
	}
	GlErrorCheck();
}

void Mandelbulb::SetUniforms(Shader& shader)
{
	shader.use();

	shader.SetUniform(power);
	shader.SetUniform(genericParameter);
	Fractal3D::SetUniforms(shader);
}

void Mandelbulb::SetUniformLocations(Shader &shader)
{
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


Shader& Mandelbulb::ParseShader(bool highQuality)
{
	std::string source = readFile(SourcePath);

	std::string base = readFile(Fractal3D::path3DBase);

	Fractal3D::ParseShader(source, base, highQuality);

	std::string vertexSource = readFile(Fractal::pathRectangleVertexshader);

	std::cout << base;

	return *(new Shader(vertexSource, base, false));
}
