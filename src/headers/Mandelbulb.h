#pragma once
#ifndef MANDELBULB_H
#define MANDELBULB_H

constexpr auto MandelSourcePath = "resources/shaders/MandelbulbT.fs";

#include "shader.h"
#include "Time.h"
#include "Camera.h"
#include "headers/Fractal.h"
#include "headers/Fractal3d.h"
class Mandelbulb : public Fractal3D
{
public:
	static const int defaultPower = 8;

	Mandelbulb(float power, std::string vertex, std::string shaderBase, Camera &camera, glm::vec3 sun, glm::ivec2 screenSize, Time time);
	Mandelbulb(float power, std::string vertex, std::string shaderBase, Camera &camera, glm::vec3 sun);
	Mandelbulb(float power, std::string vertex, std::string shaderBase, Camera &camera);
	Mandelbulb(std::string vertex, std::string shaderBase);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void SetUniforms(Shader& shader) override;
	void SetUniformLocations(Shader& shader) override;
	void SetUniformNames() override;
	void Update() override;
	static Shader& ParseShader(const std::string vertex, const std::string fragmentBasePath, bool highQuality);

	Uniform<float> power;
	Uniform<float> genericParameter;
	Uniform<glm::vec3> sun;
};

#endif
