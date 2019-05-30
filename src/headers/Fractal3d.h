#pragma once
#ifndef FRACTAL3D_H
#define FRACTAL3D_H

#include "headers/Fractal.h"
#include "headers/Camera.h"
#include "headers/Time.h"
#include <map>

struct ShaderSection
{
	std::string name;
	bool optional;
	std::string releaseName;
	ShaderSection(std::string name) : name(name), optional(false), releaseName("")
	{}
	ShaderSection(std::string name, bool optional) : name(name), optional(optional), releaseName("")
	{}
	ShaderSection(std::string name, bool optional, std::string releaseName) : name(name), optional(optional), releaseName(releaseName)
	{}

	bool operator<(const ShaderSection& c2) const
	{
		return this->name[0] < c2.name[0];
	}
};

const ShaderSection shaderSections[] = {ShaderSection("constants", true), ShaderSection("uniforms", true), ShaderSection("helperFunctions", true),
										ShaderSection("distanceEstimator"), ShaderSection("sceneDistance"), ShaderSection("trace"),
										ShaderSection("render"), ShaderSection("render"), ShaderSection("main", false, "mainAA"),
										ShaderSection("lightingFunctions", false)};

const ShaderSection postShaderSections[] = { ShaderSection("coloring"), ShaderSection("edgeGlow"), ShaderSection("color", false), };

const ShaderSection constants[] = { ShaderSection("maxIterations", false, "maxIterationsRelease"), ShaderSection("maxSteps", false, "maxStepsRelease"),
									ShaderSection("shadowSoftness", false), };

class Fractal3D : public Fractal
{
public:
	Camera& camera;
	Time time;
	Uniform<glm::vec3> sun;

	Fractal3D(Shader& explorationShader, Shader& renderShader, Camera& camera, glm::ivec2 screenSize, Time time, glm::vec3 sun);
	Fractal3D(Shader& explorationShader, Shader& renderShader);


	void MouseCallback(GLFWwindow* window, double x, double y) override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height) override;
	void SetUniforms(Shader& shader) override;
	void SetUniformLocations(Shader& shader) override;
	void SetUniformNames() override;
	void SaveImage(const std::string path) override;
	void Update() override;
	static void ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string source, std::string& final, bool highQuality);
	static void ParseShader(std::string source, std::string& final, bool highQuality);

	static const constexpr char* path3DBase = "resources/shaders/3D/3DFractalbase.fs";
	static const constexpr char* default3DFractal = "resources/shaders/3D/3DFractalDefault.fs";

private:
	glm::vec2 mouseOffset;
	bool firstMouse = true;
};

#endif