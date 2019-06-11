#pragma once
#ifndef FRACTAL_H
#define FRACTAL_H

#include "glew.h"
#include <GLFW/glfw3.h>
#include <string>

#include "headers/Uniform.h"
#include "headers/Shader.h"
#include "glm.hpp"
#include <vector>

class Shader;

struct Section
{
	std::string start;
	std::string end;

	Section(std::string name)
	{
		start = "<" + name + ">";
		end = "</" + name + ">";
	}
};

class Fractal
{
public:
	static const int DefaultWidth = 1920;
	static const int DefaultHeight = 1080;

	
	float parameterChangeRate = 1;
	Shader &explorationShader;
	Shader &renderShader;
	Uniform<glm::ivec2> screenSize;
	Fractal(Shader &explorationShader, Shader &renderShader, Uniform<glm::ivec2> screenSize) : explorationShader(explorationShader), renderShader(renderShader), screenSize(screenSize)
	{}

	virtual void MouseCallback(GLFWwindow* window, double x, double y) = 0;
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
	virtual void FramebufferSizeCallback(GLFWwindow* window, int width, int height) = 0;
	virtual void SetUniforms(Shader& shader) = 0;
	virtual void SetUniformNames() = 0;
	virtual void SetUniformLocations(Shader& shader) = 0;
	virtual void Update() = 0;
	virtual void SaveImage(std::string filePath) = 0;
	virtual void SetVariablesFromSpec(int index, std::string SpecificationPath) = 0;
	virtual void SetVariable(std::string name, std::string value) = 0;

	static const constexpr char* pathRectangleVertexshader = "resources/shaders/Rectangle.glsl";

protected:
	static bool replace(std::string& str, const std::string& from, const std::string& to);
	static bool replaceSection(Section originSection, Section destSection, std::string& origin, std::string& dest);
	static bool replaceSection(Section section, std::string& origin, std::string& dest);
	static std::string getSection(Section s, std::string from, size_t start = 0);
	static std::string getSectionName(std::string str);
	static std::string getSectionValue(std::string str);
	static void cleanString(std::string& str, std::vector<char> chars);
	static bool RemoveOuterSection(std::string& str);
	static std::vector<std::string> split(std::string str, char splitBy);
	static std::vector<std::string> splitNotInChar(std::string str, char splitBy, char opener, char closer);
	static std::string GetSpecificationByIndex(std::string specification, int index, const std::string presets);
	static void LinkSpecification(std::string& source, std::string& target);
	static void BuildDistanceEstimator(std::string& source, const std::string& defaultSource, std::string& target, std::string& specification);
	static std::vector<std::string> GetOuterSections(std::string& source);
	static std::vector<std::string> GetSections(std::string& source);
	static bool StringToBool(std::string str);
	static glm::ivec2 GetMonitorSize();
};

#endif