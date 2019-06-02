#pragma once
#ifndef MANDELBULB_H
#define MANDELBULB_H

#include "shader.h"
#include "Time.h"
#include "Camera.h"
#include "headers/Fractal.h"
#include "headers/Fractal3d.h"

const ShaderSection mandelBulbShaderSections[] = { ShaderSection("colorB", false, "", true), ShaderSection("colorC", false, "", true) };


class Mandelbulb : public Fractal3D
{
public:
	static const int defaultPower = 8;

	Mandelbulb(float power, Camera &camera, glm::vec3 sun, glm::ivec2 screenSize, Time time, int specIndex);
	Mandelbulb(float power, Camera &camera, glm::vec3 sun, int specIndex);
	Mandelbulb(float power, Camera &camera, int specIndex);
	Mandelbulb(int specIndex);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void SetUniforms(Shader& shader) override;
	void SetUniformLocations(Shader& shader) override;
	void SetUniformNames() override;
	void Update() override;
	static Shader& GenerateShader(bool highQuality, int specIndex);
	void SetVariablesFromSpec(int index);
	void SetVariable(std::string name, std::string value) override;

	Uniform<float> power;
	Uniform<float> genericParameter;
private:
	static constexpr const char* SourcePath = "resources/shaders/3D/Mandelbulb.fs";
	static constexpr const char* SpecificationPath = "resources/shaders/3D/MandelbulbSpecs.fs";
};

#endif
