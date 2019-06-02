#pragma once
#ifndef Mandelbox_H
#define Mandelbox_H

#include "shader.h"
#include "Time.h"
#include "Camera.h"
#include "headers/Fractal.h"
#include "headers/Fractal3d.h"

const ShaderSection mandelBoxShaderSections[] = { ShaderSection("innerRadius"), ShaderSection("outerRadius"),
												  ShaderSection("foldingLimit"), ShaderSection("scale") };


class Mandelbox : public Fractal3D
{
public:
	static const int defaultPower = 1;

	Mandelbox(float power, Camera& camera, glm::vec3 sun, glm::ivec2 screenSize, Time time, int specIndex);
	Mandelbox(float power, Camera& camera, glm::vec3 sun, int specIndex);
	Mandelbox(float power, Camera& camera, int specIndex);
	Mandelbox(int specIndex);
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
	static constexpr const char* SourcePath = "resources/shaders/3D/Mandelbox.fs";
	static constexpr const char* SpecificationPath = "resources/shaders/3D/MandelboxSpecs.fs";
};

#endif
