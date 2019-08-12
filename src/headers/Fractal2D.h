#pragma once
#ifndef FRACTAL2D_H
#define FRACTAL2D_H

#include "Fractal.h"

const ShaderSection shaderSections2D[] = { ShaderSection("constants", true), ShaderSection("uniforms", true), ShaderSection("buffers", true),
										   ShaderSection("main", false, "mainAA"),};

const ShaderSection postShaderSections2D[] = { ShaderSection("coloring", false, "", true), ShaderSection("color", false, "", true),};


const ShaderSection constants2D[] = { ShaderSection("maxIterations", false, "maxIterationsRelease"), ShaderSection("antiAliasing"),
									  ShaderSection("escapeRadius"), };

class Fractal2D : public Fractal
{
public:
	Uniform<float> power;
	Uniform<glm::vec2> position;

	Uniform<glm::vec2> mousePosition;

	Fractal2D(int specIndex, int fractalIndex, int fractalNameIndex, glm::ivec2 screenSize);
	Fractal2D(int specIndex, int fractalIndex, int fractalNameIndex);

	void Update() override;
	void MouseCallback(GLFWwindow* window, double x, double y) override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height) override;
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) override {}
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
	void ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string & final, std::string specification, bool highQuality);
	void ParseShader(std::string& source, std::string & final, const std::string* spec, bool highQuality, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections);
	void Init();
	static std::map<std::string, int*> GetDefaultShaderIndices();
	void RenderComputeShader();


	static const constexpr char* fractal2dPath = "shaders/2D/Fractals/";
	static const constexpr char* path2DBase = "shaders/2D/Base/2DFractalBase.fs";
	static const constexpr char* path2DBaseCompute = "shaders/2D/Base/2DFractalBaseCompute.fs";
	static const constexpr char* presetSpec2D = "shaders/2D/Base/PresetSpecs.fs";
	static const constexpr char* default2DFractal = "shaders/2D/Base/2DFractalDefault.fs";
	static const constexpr char* helperFunctions = "shaders/2D/Base/HelperFunctions.fs";
	static const constexpr char* alternateDefaultFunctionsPath = "shaders/2D/Base/AlternateDefault.fs";

	static const constexpr char* computeRenderBufferName = "renderInput";

private:
	const static std::string& default2DSource;
	Shader* CreateShader(std::string source, const std::string* specification, bool highQuality, int* fractalIndex, int* specIndex, std::vector<ShaderSection> shaderSections);
};

#endif



