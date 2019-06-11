#pragma once
#ifndef FRACTAL3D_H
#define FRACTAL3D_H

#include "headers/Fractal.h"
#include "headers/Camera.h"
#include "headers/Time.h"
#include <map>
#include <thread>

struct ShaderSection
{
	std::string name;
	bool optional;
	std::string releaseName;
	bool multiple;
	ShaderSection(std::string name) : name(name), optional(false), releaseName(""), multiple(false)
	{}
	ShaderSection(std::string name, bool optional) : name(name), optional(optional), releaseName(""), multiple(false)
	{}
	ShaderSection(std::string name, bool optional, std::string releaseName) : name(name), optional(optional), releaseName(releaseName), multiple(false)
	{}
	ShaderSection(std::string name, bool optional, std::string releaseName, bool multiple) : name(name), optional(optional), releaseName(releaseName), multiple(multiple)
	{}

	bool operator<(const ShaderSection& c2) const
	{
		return this->name[0] < c2.name[0];
	}
};

const ShaderSection shaderSections[] = {ShaderSection("constants", true), ShaderSection("uniforms", true),
										ShaderSection("sceneDistance"), ShaderSection("trace"),
										ShaderSection("render"), ShaderSection("render"), ShaderSection("main", false, "mainAA"),
										ShaderSection("lightingFunctions") };

const ShaderSection postShaderSections[] = { ShaderSection("coloring"), ShaderSection("edgeGlow", true), ShaderSection("sky", true), ShaderSection("sun", true),
											 ShaderSection("color", false, "", true), ShaderSection("maxDist", false, "maxDistRelease"), ShaderSection("distanceBody"), ShaderSection("trap") };

const ShaderSection constants[] = { ShaderSection("maxIterations", false, "maxIterationsRelease"), ShaderSection("maxSteps", false, "maxStepsRelease"),
									ShaderSection("shadowSoftness"), ShaderSection("antiAliasing"), ShaderSection("zoom") };

const ShaderSection cpuVariables[] = { ShaderSection("position"), ShaderSection("sun", false, "maxStepsRelease"),
										ShaderSection("shadowSoftness", false), ShaderSection("antiAliasing", false), };

class Fractal3D : public Fractal
{
public:
	Camera& camera;
	Time time;
	Uniform<glm::vec3> sun;
	Uniform<float> power;
	Uniform<float> genericParameter;

	std::map<int, bool> keys;


	Fractal3D(float power, Shader& explorationShader, Shader& renderShader, Camera& camera, glm::vec3 sun, glm::ivec2 screenSize, Time time, int specIndex, std::string specification);
	Fractal3D(int specIndex, std::string specPath, std::string sourcePath);


	void MouseCallback(GLFWwindow* window, double x, double y) override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height) override;
	void SetUniforms(Shader& shader) override;
	void SetUniformLocations(Shader& shader) override;
	void SetUniformNames() override;
	void SaveImage(const std::string path) override;
	void Update() override;
	void SetVariable(std::string name, std::string value) override;
	void SetVariablesFromSpec(int index, std::string specification) override;
	void HandleKeyInput();
	static Shader& GenerateShader(bool highQuality, int specIndex, std::string specification, std::string source);
	static void ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string& final, std::string specification, bool highQuality);
	static void ParseShader(std::string& source, std::string& final, std::string spec, bool highQuality, int specIndex, const std::vector<ShaderSection> extraSections);


	static const constexpr char* path3DBase = "resources/shaders/3D/Base/3DFractalbase.fs";
	static const constexpr char* default3DFractal = "resources/shaders/3D/Base/3DFractalDefault.fs";
	static const constexpr char* helperFunctions = "resources/shaders/3D/Base/HelperFunctions.fs";
	static const constexpr char* presetSpec = "resources/shaders/3D/Base/PresetSpecs.fs";

private:
	const static std::string& default3DSource;
	void Init(int specIndex, std::string specification);
	glm::vec2 mouseOffset;
	bool firstMouse = true;
};

#endif