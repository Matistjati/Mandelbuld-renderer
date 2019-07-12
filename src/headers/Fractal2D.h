#pragma once
#ifndef FRACTAL2D_H
#define FRACTAL2D_H

#include "Fractal.h"

class Fractal2D : public Fractal
{
public:
	Uniform<float> power;
	Uniform<glm::vec2> position;

	glm::vec2 mousePosition;

	Fractal2D(int specIndex, int fractalIndex, int fractalNameIndex, glm::ivec2 screenSize);
	Fractal2D(int specIndex, int fractalIndex, int fractalNameIndex);

	void Update() override;
	void MouseCallback(GLFWwindow* window, double x, double y) override; // TODO
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height) override;
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) override {}
	void SetUniforms(Shader* shader) override;
	void SetUniformLocations(Shader* shader) override;
	void SetUniformNames() override;
	void SaveImage(const std::string path) override;
	void FindPathAndSaveImage() override;
	void SetVariable(std::string name, std::string value) override; // TODO
	void SetVariablesFromSpec(int* index, std::string specification) override; // TODO
	void HandleKeyInput() override;
	std::pair<Shader*, Shader*> GenerateShader(int* specIndex, int* fractalIndex, std::string name) override;
	std::pair<Shader*, Shader*> GenerateShader() override;
	std::pair<Shader*, Shader*> GenerateShader(std::string fractalName) override;
	std::pair<Shader*, Shader*> GenerateShader(int specIndex, int fractalIndex, std::string fractalName) override;
	std::string GetSpecPath(std::string fileName) override;
	std::string GetFractalPath(std::string fileName) override;
	std::string GetFractalFolderPath() override;
	void ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string & final, std::string specification, bool highQuality);
	void ParseShader(std::string& source, std::string & final, const std::string* spec, bool highQuality, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections);
	void Init();
	std::map<std::string, int*> GetDefaultShaderIndices() override; // TODO

	static const constexpr char* fractal2dPath = "shaders/2D/Fractals/";
	static const constexpr char* path2DBase = "shaders/2D/Base/2DFractalBase.fs";

private:
};

#endif



