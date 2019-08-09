#pragma once
#ifndef FRACTAL3D_H
#define FRACTAL3D_H

#include "headers/Fractal.h"
#include "headers/Camera.h"
#include "headers/Time.h"
#include <map>
#include <thread>

const double scrollSpeed = 10;

const ShaderSection shaderSections[] = {ShaderSection("constants", true), ShaderSection("uniforms", true),
										ShaderSection("sceneDistance"), ShaderSection("trace"),
										ShaderSection("render"), ShaderSection("render"), ShaderSection("main", false, "mainAA"),
										ShaderSection("lightingFunctions") };

const ShaderSection postShaderSections[] = { ShaderSection("coloring", false, "", true), ShaderSection("edgeGlow", true), ShaderSection("sky", true), ShaderSection("sun", true),
											 ShaderSection("color", false, "", true), ShaderSection("maxDist", false, "maxDistRelease"), ShaderSection("distanceBody"), ShaderSection("trap") };

const ShaderSection constants[] = { ShaderSection("maxIterations", false, "maxIterationsRelease"), ShaderSection("maxSteps", false, "maxStepsRelease"),
									ShaderSection("shadowSoftness"), ShaderSection("antiAliasing"), ShaderSection("zoomDetailRatio") };

class Fractal3D : public Fractal
{
public:
	Camera& camera;
	Uniform<glm::vec3> sun;
	Uniform<float> power;
	Uniform<float> genericParameter;


	Fractal3D(float power, Shader* explorationShader, Shader* renderShader, Camera& camera, glm::vec3 sun, glm::ivec2 screenSize, Time time, int* specIndex, std::string specification);
	Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex, glm::ivec2 screenSize);
	Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex);
	float GetZoom() { return 1 / zoom.value; };

	void Update() override;
	void MouseCallback(GLFWwindow* window, double x, double y) override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height) override;
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;
	void SetUniforms(Shader* shader) override;
	void SetUniformLocations(Shader* shader) override;
	void SetUniformNames() override;
	void SaveImage(const std::string path) override;
	void FindPathAndSaveImage() override;
	void SetVariable(std::string name, std::string value) override;
	void SetVariablesFromSpec(int* index, std::string specification) override;
	void HandleKeyInput() override;
	std::pair<Shader*, Shader*> GenerateShader(int* specIndex, int* fractalIndex, std::string name) override;
	std::pair<Shader*, Shader*> GenerateShader() override;
	std::pair<Shader*, Shader*> GenerateShader(std::string fractalName) override;
	std::pair<Shader*, Shader*> GenerateShader(int specIndex, int fractalIndex, std::string fractalName) override;
	std::string GetSpecPath(std::string fileName) override;
	std::string GetFractalPath(std::string fileName) override;
	std::string GetFractalFolderPath() override;
	void ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string& final, std::string specification, bool highQuality);
	void ParseShader(std::string& source, std::string& final, const std::string* spec, bool highQuality, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections);
	void Init();
	static std::map<std::string, int*> GetDefaultShaderIndices();

	static const constexpr char* path3DBase = "shaders/3D/Base/3DFractalbase.fs";
	static const constexpr char* default3DFractal = "shaders/3D/Base/3DFractalDefault.fs";
	static const constexpr char* helperFunctions = "shaders/3D/Base/HelperFunctions.fs";
	static const constexpr char* presetSpec3D = "shaders/3D/Base/PresetSpecs.fs";
	static const constexpr char* alternateDefaultFunctionsPath = "shaders/3D/Base/AlternateDefault.fs";
	static const constexpr char* fractal3dPath = "shaders/3D/Fractals/";

private:
	const static std::string& default3DSource;
	glm::vec2 mouseOffset;
	bool firstMouse = true;
};

#endif