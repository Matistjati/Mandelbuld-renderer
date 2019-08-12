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
#include <string>

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

	// Required for some templates
	bool operator<(const ShaderSection& c2) const
	{
		return this->name[0] < c2.name[0];
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
	static Uniform<glm::ivec2> screenSize;
	static GLFWwindow* window;
	float parameterChangeRate = 1;
	Shader* explorationShader;
	Shader* renderShader;
	Uniform<float> zoom;
	Uniform<Time> time;
	Uniform<unsigned int> frame;

	FractalType fractalType;
	int fractalIndex;
	int specIndex;
	int fractalNameIndex;
	std::string fractalName;

	std::map<int, bool> keys;
	std::map<std::string, int*> shaderIndices;

	~Fractal()
	{
		delete explorationShader;
		delete renderShader;
	};

	// Nothing fancy
	Fractal(std::pair<Shader*, Shader*> shaders, Uniform<glm::ivec2> screenSize, Time t, std::map<std::string, int*> shaderIndices, float zoom = 1, FractalType f = FractalType::error, int fractalIndex = 0,
		int specIndex = 0, int fractalNameIndex = 0, std::string fractalName = "")
		: explorationShader(shaders.first), renderShader(shaders.second), zoom(zoom), fractalType(f), time(t, "time", glGetUniformLocation(shaders.first->id, "time")), fractalIndex(fractalIndex), specIndex(specIndex),
		fractalName(fractalName), fractalNameIndex(fractalNameIndex), shaderIndices(shaderIndices)
	{
		Fractal::screenSize = screenSize;
	}

	static void RenderLoop(GLFWwindow* window, Fractal* fractal);
	static void GenerateSingleImage(GLFWwindow* window, Fractal* fractal);
	void SetFractalNameFromIndex(int* index, std::string fractalPath);
	void UpdateFractalShader();
	static glm::ivec2 GetMonitorSize();

	virtual void Update() = 0;
	virtual void MouseCallback(GLFWwindow* window, double x, double y) = 0;
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
	virtual void FramebufferSizeCallback(GLFWwindow* window, int width, int height) = 0;
	virtual void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) = 0;
	virtual void SetUniformLocations(Shader* shader) = 0;
	virtual void SetUniforms(Shader* shader) = 0;
	virtual void SetUniformNames() = 0;
	virtual void SaveImage(std::string filePath) = 0;
	virtual void FindPathAndSaveImage() = 0;
	virtual void SetVariablesFromSpec(int* index, std::string SpecificationPath) = 0;
	virtual void SetVariable(std::string name, std::string value) = 0;
	virtual void HandleKeyInput() = 0;
	virtual std::string GetSpecPath(std::string fileName) = 0;
	virtual std::string GetFractalPath(std::string fileName) = 0;
	virtual std::pair<Shader*, Shader*> GenerateShader(int* specIndex, int* fractalIndex, std::string name) = 0;
	virtual std::pair<Shader*, Shader*> GenerateShader(std::string name) = 0;
	virtual std::pair<Shader*, Shader*> GenerateShader() = 0;
	virtual std::pair<Shader*, Shader*> GenerateShader(int specIndex, int fractalIndex, std::string fractalName) = 0;
	virtual std::string GetFractalFolderPath() = 0;


	static const constexpr char* pathRectangleVertexshader = "shaders/Rectangle.glsl";
	static const constexpr char* rectangleVertexBufferName = "vertices";
	static const constexpr char* rectangleVertexBufferIndexName = "verticesIndex";

	static const constexpr char* shaderSuffixes[] = { "Render", "MapImage", "Specs" };

protected:
	static bool Replace(std::string& str, const std::string& from, const std::string& to);
	static bool ReplaceSection(Section originSection, Section destSection, std::string& origin, std::string& dest);
	static bool ReplaceSection(Section section, std::string& origin, std::string& dest);
	static std::string GetSection(Section s, std::string from, size_t start = 0);
	static std::string GetWholeSection(Section s, std::string from, size_t start = 0);
	static std::string GetSectionName(std::string str);
	static std::string GetSectionValue(std::string str);
	static void CleanString(std::string& str, std::vector<char> chars);
	static bool RemoveOuterSection(std::string& str);
	static bool RemoveOuterChars(std::string& str, char first, char last);
	static std::vector<std::string> Split(std::string str, char splitBy);
	static std::vector<std::string> SplitNotInChar(std::string str, char splitBy, char opener, char closer);
	static std::vector<std::string> SplitNotInChar(std::string str, char splitBy, std::vector<std::pair<char, char>> ignore);
	static std::string GetSpecificationByIndex(const std::string* specification, int* index, const std::string presets);
	static void LinkSpecification(std::string& source, std::string& target);
	static void BuildMainLoop(Section targetSection, std::string& source, const std::string& defaultSource, std::string& target, std::string& specification, int* index, std::map<std::string, int*> indices);
	static void BuildMainLoop(Section targetSection, std::string& source, const std::string& defaultSource, std::string& target, std::string& specification, std::map<std::string, int*> indices);
	static std::vector<std::string> GetOuterSections(std::string& source);
	static std::vector<std::string> GetSections(std::string& source);
	static bool StringToBool(std::string str);
	static std::vector<std::string> GetFractalNames(std::vector<std::string> names);
};

#endif