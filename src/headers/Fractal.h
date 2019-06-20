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
#include <utility>

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
	Shader* explorationShader;
	Shader* renderShader;
	Uniform<glm::ivec2> screenSize;
	Uniform<float> zoom;
	Time time;

	FractalType fractalType;
	int fractalIndex;
	int specIndex;
	std::string fractalName;

	std::map<int, bool> keys;

	~Fractal()
	{
		delete explorationShader;
		delete renderShader;
	};

	// Nothing fancy
	Fractal(std::pair<Shader*, Shader*> shaders, Uniform<glm::ivec2> screenSize, Time t, float zoom = 1, FractalType f = FractalType::error, int fractalIndex = -1,
		int specIndex = -1, std::string fractalName = "")
		: explorationShader(shaders.first), renderShader(shaders.second), screenSize(screenSize), zoom(zoom), fractalType(f), time(t), fractalIndex(fractalIndex), specIndex(specIndex),
		fractalName(fractalName)
	{}

	virtual void MouseCallback(GLFWwindow* window, double x, double y) = 0;
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
	virtual void FramebufferSizeCallback(GLFWwindow* window, int width, int height) = 0;
	virtual void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) = 0;
	virtual void SetUniformLocations(Shader* shader) = 0;
	virtual void SetUniforms(Shader* shader) = 0;
	virtual void SetUniformNames() = 0;
	virtual void Update() = 0;
	virtual void SaveImage(std::string filePath) = 0;
	virtual void SetVariablesFromSpec(int* index, std::string SpecificationPath) = 0;
	virtual void SetVariable(std::string name, std::string value) = 0;
	virtual void HandleKeyInput() = 0;
	virtual std::string GetSpecPath(std::string fileName) = 0;
	virtual std::string GetFractalPath(std::string fileName) = 0;
	virtual std::pair<Shader*, Shader*> GenerateShader(int* specIndex, int* fractalIndex, std::string name) = 0;
	virtual std::pair<Shader*, Shader*> GenerateShader() = 0;

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
	static std::string GetSpecificationByIndex(std::string specification, int* index, const std::string presets);
	static void LinkSpecification(std::string& source, std::string& target);
	static void BuildDistanceEstimator(std::string& source, const std::string& defaultSource, std::string& target, std::string& specification, int* index);
	static void BuildDistanceEstimator(std::string& source, const std::string& defaultSource, std::string& target, std::string& specification);
	static std::vector<std::string> GetOuterSections(std::string& source);
	static std::vector<std::string> GetSections(std::string& source);
	static bool StringToBool(std::string str);
};

#endif