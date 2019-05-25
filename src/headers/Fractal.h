#pragma once
#ifndef FRACTAL_H
#define FRACTAL_H

#include "glew.h"
#include <GLFW/glfw3.h>
#include <string>

#include "headers/Uniform.h"
#include "headers/Shader.h"
#include "glm.hpp"

class Shader;

class Fractal
{
public:
	static const int DefaultWidth = 1920;
	static const int DefaultHeight = 1040;

	Shader &explorationShader;
	Shader &renderShader;
	Uniform<glm::ivec2> screenSize;
	Fractal(Shader &explorationShader, Shader &renderShader, Uniform<glm::ivec2> screenSize) : explorationShader(explorationShader), renderShader(renderShader), screenSize(screenSize)
	{}

	virtual void MouseCallback(GLFWwindow* window, double x, double y) = 0;
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
	virtual void FramebufferSizeCallback(GLFWwindow* window, int width, int height) = 0;
	virtual void SetUniforms(Shader& shader) = 0;
	virtual void SetUniformNames() = 0;
	virtual void SetUniformLocations(Shader& shader) = 0;
	virtual void Update() = 0;
	virtual void SaveImage(std::string filePath) = 0;
};

#endif