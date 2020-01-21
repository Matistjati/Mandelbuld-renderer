#pragma once
#ifndef FRACTAL3D_H
#define FRACTAL3D_H

#include "headers/Fractal.h"
#include "headers/Camera.h"
#include "headers/Time.h"
#include <map>
#include <thread>

const ShaderSection shaderSections[] = { ShaderSection("constants", true), ShaderSection("uniforms", true),
										 ShaderSection("sceneDistance"), ShaderSection("trace"),
										 ShaderSection("render"), ShaderSection("render"), ShaderSection("main", false),
										 ShaderSection("lightingFunctions") };

const ShaderSection postShaderSections[] = { ShaderSection("coloring", false, true), ShaderSection("edgeGlow", false, true), ShaderSection("sky", true), ShaderSection("sun", true),
											 ShaderSection("distanceBody"), ShaderSection("trap") };

class Fractal3D : public Fractal
{
public:
	bool cursorVisible;
	Camera& camera;
	Uniform<glm::vec3> sun;
	glm::dvec2 lastNonGuiPos;

	Fractal3D(float power, Shader* explorationShader, Shader* renderShader, Camera& camera, glm::vec3 sun, glm::vec2 screenSize, Time time, int* specIndex, std::string specification);
	Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex, glm::vec2 screenSize);
	Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex);

	void PopulateGUI() override;
	void Update() override;
	void MouseCallback(GLFWwindow* window, double x, double y) override;
	void MousePressCallback(GLFWwindow* window, int button, int action, int mods) override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height) override;
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;
	void SetUniforms(Shader* shader, bool computeRender = false) override;
	void SetUniformLocations(Shader* shader, bool computeRender = false) override;
	void SetUniformNames() override;
	void SetVariable(std::string name, std::string value) override;
	void HandleKeyInput() override;
	Shader* GenerateShader(int* specIndex, int* fractalIndex, std::string name) override;
	Shader* GenerateShader() override;
	Shader* GenerateShader(std::string fractalName) override;
	Shader* GenerateShader(int specIndex, int fractalIndex, std::string fractalName) override;
	std::string GetSpecPath(std::string fileName) override;
	std::string GetFractalPath(std::string fileName) override;
	std::string GetFractalFolderPath() override;
	void ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string& final, std::string specification);
	void ParseShader(std::string& source, std::string& final, const std::string* spec, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections);
	void Init() override;
	static std::map<std::string, int*> GetDefaultShaderIndices();
	void SetShaderGui(bool render) override;
	virtual void SetShaderUniforms(bool render);

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