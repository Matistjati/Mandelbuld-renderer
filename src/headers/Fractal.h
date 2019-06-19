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
#include <map>
#include <headers/Time.h>

const double scrollSpeed = 10;

class Shader;

struct Section
{
	std::string start;
	std::string end;

	Section(std::string name)
	{
		start = ("<" + name) + ">";
		end = ("</" + name) + ">";
	}
};

enum FractalType
{
	error = -1,
	fractal3D = 0,
	fractal2D = 1,
};

class Fractal
{
public:
	float parameterChangeRate = 1;
	Shader &explorationShader;
	Shader &renderShader;
	Uniform<glm::ivec2> screenSize;
	Uniform<float> zoom;
	Time time;

	FractalType fractalType;

	std::map<int, bool> keys;

	Fractal(Shader &explorationShader, Shader &renderShader, Uniform<glm::ivec2> screenSize, Time t, float zoom = 1) : explorationShader(explorationShader), renderShader(renderShader),
		screenSize(screenSize), zoom(zoom), fractalType(FractalType::error), time(t)
	{}

	virtual void MouseCallback(GLFWwindow* window, double x, double y) {};
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {};
	virtual void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {};
	virtual void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {};
	virtual void SetUniforms(Shader& shader) {};
	virtual void SetUniformNames() {};
	virtual void SetUniformLocations(Shader& shader) {};
	virtual void Update() {};
	virtual void SaveImage(std::string filePath) {};
	virtual void SetVariablesFromSpec(int index, std::string SpecificationPath) {};
	virtual void SetVariable(std::string name, std::string value) {};
	virtual void HandleKeyInput() {};

	static const constexpr char* pathRectangleVertexshader = "shaders/Rectangle.glsl";

	static glm::ivec2 GetMonitorSize();

protected:
	static bool replace(std::string& str, const std::string& from, const std::string& to);
	static bool replaceSection(Section originSection, Section destSection, std::string& origin, std::string& dest);
	static bool replaceSection(Section section, std::string& origin, std::string& dest);
	static std::string getSection(Section s, std::string from, size_t start = 0);
	static std::string getSectionName(std::string str);
	static std::string getSectionValue(std::string str);
	static void cleanString(std::string& str, std::vector<char> chars);
	static bool RemoveOuterSection(std::string& str);
	static bool RemoveOuterChars(std::string& str, char first, char last);
	static std::vector<std::string> split(std::string str, char splitBy);
	static std::vector<std::string> splitNotInChar(std::string str, char splitBy, char opener, char closer);
	static std::string GetSpecificationByIndex(std::string specification, int index, const std::string presets);
	static void LinkSpecification(std::string& source, std::string& target);
	static void BuildDistanceEstimator(std::string& source, const std::string& defaultSource, std::string& target, std::string& specification);
	static std::vector<std::string> GetOuterSections(std::string& source);
	static std::vector<std::string> GetSections(std::string& source);
	static bool StringToBool(std::string str);
};

#endif