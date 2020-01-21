#pragma once
#ifndef FRACTAL2D_H
#define FRACTAL2D_H

#include "Fractal.h"

const ShaderSection shaderSections2D[] = { ShaderSection("constants", true), ShaderSection("uniforms", true), ShaderSection("buffers", true),
										   ShaderSection("main", false), };

const ShaderSection postShaderSections2D[] = { ShaderSection("coloring", false, true) };

class Fractal2D : public Fractal
{
public:
	Uniform<glm::vec2> position;
	// XY is mouse position in fractal space, ZY is the exploration position
	Uniform<glm::vec2> clickPosition;

	Uniform<glm::vec2> mousePosition;

	Fractal2D(int specIndex, int fractalIndex, int fractalNameIndex, glm::vec2 screenSize);
	Fractal2D(int specIndex, int fractalIndex, int fractalNameIndex);

	void PopulateGUI() override;
	void Update() override;
	void MouseCallback(GLFWwindow* window, double x, double y) override;
	void MousePressCallback(GLFWwindow* window, int button, int action, int mods) override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height) override;
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) override {}
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
	void ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string & final, std::string specification);
	void ParseShader(std::string& source, std::string & final, const std::string* spec, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections);
	void Init() override;
	static std::map<std::string, int*> GetDefaultShaderIndices();
	void RenderComputeShader();
	void SetShaderGui(bool render) override;
	virtual void SetShaderUniforms(bool render);
	glm::vec2 MapScreenMouseToFractal();

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
	Shader* CreateShader(std::string source, const std::string* specification, int* fractalIndex, int* specIndex, std::vector<ShaderSection> shaderSections);
};

#endif



