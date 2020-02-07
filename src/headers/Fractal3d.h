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
	Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex, glm::vec2 screenSize);
	Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex);

	Shader* GenerateShader(int* specIndex, int* fractalIndex, std::string name) override;
	Shader* GenerateShader() override;
	Shader* GenerateShader(std::string fractalName) override;
	Shader* GenerateShader(int specIndex, int fractalIndex, std::string fractalName) override;
	std::string GetSpecPath(std::string fileName) override;
	std::string GetFractalPath(std::string fileName) override;
	std::string GetFractalFolderPath() override;
	void ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string& final, std::string specification);
	void ParseShader(std::string& source, std::string& final, const std::string* spec, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections);

	static const constexpr char* path3DBase = "shaders/3D/Base/3DFractalbase.fs";
	static const constexpr char* default3DFractal = "shaders/3D/Base/3DFractalDefault.fs";
	static const constexpr char* helperFunctions = "shaders/3D/Base/HelperFunctions.fs";
	static const constexpr char* presetSpec3D = "shaders/3D/Base/PresetSpecs.fs";
	static const constexpr char* alternateDefaultFunctionsPath = "shaders/3D/Base/AlternateDefault.fs";
	static const constexpr char* fractal3dPath = "shaders/3D/Fractals/";

private:
	const static std::string& default3DSource;
};

#endif
