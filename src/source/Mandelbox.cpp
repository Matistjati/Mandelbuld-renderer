#include "headers/Mandelbox.h"
#include <windows.h>
#include "glm.hpp"
#include "headers/shader.h"
#include "headers/camera.h"
#include <glew.h>
#include <iostream>
#include "headers/Image.h"
#include <sys/stat.h>
#include "headers/GlError.h"
#include <string>
#include <fstream>

Mandelbox::Mandelbox(float power, std::string vertex, std::string shaderBase, Camera& camera, glm::vec3 sun, glm::ivec2 screenSize, Time time)
	: Fractal3D(ParseShader(vertex, shaderBase, false), ParseShader(vertex, shaderBase, true), camera, screenSize, time), power(power), genericParameter(1), sun(sun)
{
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbox::Mandelbox(float power, std::string vertex, std::string shaderBase, Camera& camera, glm::vec3 sun)
	: Fractal3D(ParseShader(vertex, shaderBase, false), ParseShader(vertex, shaderBase, true), camera, glm::ivec2(Fractal::DefaultWidth, Fractal::DefaultHeight), Time()), power(power), genericParameter(1), sun(sun)
{
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbox::Mandelbox(float power, std::string vertex, std::string shaderBase, Camera& camera)
	: Fractal3D(ParseShader(vertex, shaderBase, false), ParseShader(vertex, shaderBase, true), camera, glm::ivec2(Fractal::DefaultWidth, Fractal::DefaultHeight), Time()), power(power), genericParameter(1), sun(glm::normalize(glm::dvec3(0.577, 0.577, 0.577)))
{
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbox::Mandelbox(std::string vertex, std::string shaderBase)
	: Fractal3D(ParseShader(vertex, shaderBase, false), ParseShader(vertex, shaderBase, true)), power(defaultPower), genericParameter(1), sun(glm::normalize(glm::dvec3(0.577, 0.577, 0.577)))
{
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

void Mandelbox::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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
		genericParameter.value -= 0.1f;
		explorationShader.SetUniform(genericParameter);
		break;


	default:
		break;
	}
	GlErrorCheck();
}

void Mandelbox::SetUniforms(Shader& shader)
{
	shader.use();

	shader.SetUniform(power);
	shader.SetUniform(sun);
	shader.SetUniform(genericParameter);
	Fractal3D::SetUniforms(shader);
}

void Mandelbox::SetUniformLocations(Shader& shader)
{
	power.id = glGetUniformLocation(shader.id, power.name.c_str());
	sun.id = glGetUniformLocation(shader.id, sun.name.c_str());
	genericParameter.id = glGetUniformLocation(shader.id, genericParameter.name.c_str());
	Fractal3D::SetUniformLocations(shader);
}

void Mandelbox::SetUniformNames()
{
	sun.name = "sun";
	power.name = "power";
	genericParameter.name = "genericParameter";
	genericParameter.value = 2;
	Fractal3D::SetUniformNames();
}

void Mandelbox::Update()
{
	time.PollTime();


	// Move sun in shader
	double time = glfwGetTime();
	sun.value = glm::normalize(glm::vec3(sin(time * 0.25),
		std::abs(sin(time * 0.1)) * -1,
		cos(time * 0.25)));

	//genericParameter.value = (float)(abs(cos(time * 0.025) * 0.5 - sin(time * 0.01) * 2));
	explorationShader.SetUniform(sun);
	explorationShader.SetUniform(genericParameter);
}


Shader& Mandelbox::ParseShader(const std::string vertex, const std::string fragmentBasePath, bool highQuality)
{
	std::ifstream t(SourcePath);
	std::string mandel((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	t = std::ifstream(fragmentBasePath);
	std::string fragmentBase((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	Section Uniforms = Section("uniforms");
	replaceSection(Uniforms, mandel, fragmentBase);

	Section Const = Section("constants");
	replaceSection(Const, mandel, fragmentBase);

	Section DE = Section("DE");
	replaceSection(DE, mandel, fragmentBase);

	Section Color = Section("Color");
	replaceSection(Color, mandel, fragmentBase);

	if (highQuality)
	{
		Section main = Section("mainAA");
		replace(fragmentBase, "%maxIter%", "4095");
		replace(fragmentBase, "%main%", "");
		replaceSection(main, mandel, fragmentBase);
	}
	else
	{
		Section main = Section("main");
		replace(fragmentBase, "%maxIter%", "4");
		replace(fragmentBase, "%mainAA%", "");
		replaceSection(main, mandel, fragmentBase);
	}


	std::cout << fragmentBase;
	t = std::ifstream(vertex);
	std::string vertexSource((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	return *(new Shader(vertexSource, fragmentBase, false));
}
