#pragma once
#ifndef FRACTAL_H
#define FRACTAL_H

#include "headers/GUI.h"
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
#include <nanogui/nanogui.h>


class Shader;
class GUI;
class GuiElement;
class SubMenu;

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
	bool multiple;
	ShaderSection(std::string name) : name(name), optional(false), multiple(false)
	{}
	ShaderSection(std::string name, bool optional) : name(name), optional(optional), multiple(false)
	{}
	ShaderSection(std::string name, bool optional, bool multiple) : name(name), optional(optional), multiple(multiple)
	{}

	// Required for some templates
	bool operator<(const ShaderSection& c2) const
	{
		return this->name[0] < c2.name[0];
	}
};

struct ShaderIndice
{
	int* value;
	int modifiers;
	std::pair<int, int> button;

	ShaderIndice(int value, int modifiers, std::pair<int, int> button) : value(new int(value)), modifiers(modifiers), button(button) {}
	ShaderIndice(int value, std::pair<int, int> button) : value(new int(value)), modifiers(0), button(button) {}
	ShaderIndice() : value(), modifiers(), button() {}

	bool HasModifier(int modifier)
	{
		return (modifiers & modifier) == modifier;
	}
};

enum class FractalType
{
	error = -1,
	fractal3D = 0,
	fractal2D = 1,
};


class Fractal
{
public:
	static bool renderMode;

	static Uniform<glm::vec2> screenSize;
	static GLFWwindow* window;
	float parameterChangeRate = 1;
	Shader* shader;
	Uniform<float> zoom;
	Uniform<Time> time;
	Uniform<float> deltaTime;
	Uniform<unsigned int> frame;

	FractalType fractalType;
	int fractalIndex;
	int specIndex;
	int fractalNameIndex;
	std::string fractalName;

	std::map<int, bool> keys;
	std::map<std::string, ShaderIndice*> shaderIndices;
	bool holdingMouse;

	GUI* gui;

	std::vector<GuiElement> fractalUniforms;
	std::vector<SubMenu*> subMenus;
	std::string fractalSourceCode;

	~Fractal();

	// Nothing fancy
	Fractal(Uniform<glm::vec2> screenSize, Time t, float zoom = 1, FractalType f = FractalType::error, int fractalIndex = 0,
		int specIndex = 0, int fractalNameIndex = 0, std::string fractalName = "");

	static void RenderLoop(GLFWwindow* window, Fractal* fractal);
	static void GenerateSingleImage(GLFWwindow* window, Fractal* fractal);
	static void ImageSequence(GLFWwindow* window, Fractal* fractal);
	void SetFractalNameFromIndex(int* index, std::string fractalPath);
	void SetVariablesFromSpec(int* index, std::string SpecificationPath, std::string presetPath);
	void UpdateFractalShader();
	void PopulateGuiFromShader();
	virtual void SetShaderUniforms(bool render);
	virtual void SetShaderGui(bool render);
 	static glm::vec2 GetMonitorSize();
	void FindPathAndSaveImage();
	void SaveImage(std::string path);
	void RenderComputeShader();

	static std::string GetSpecificationByIndex(const std::string* specification, int* index, const std::string presets);
	static void LinkSpecification(std::string& source, std::string& target);
	static void BuildMainLoop(Section targetSection, std::string& source, const std::string& defaultSource, std::string& target, std::string& specification, int* index, std::map<std::string, ShaderIndice*> indices);
	static void BuildMainLoop(Section targetSection, std::string& source, const std::string& defaultSource, std::string& target, std::string& specification, std::map<std::string, ShaderIndice*> indices);
	void AddShaderParameters(std::string& spec);

	virtual void PopulateGUI();
	virtual void Update();
	virtual void MouseCallback(GLFWwindow* window, double x, double y) = 0;
	virtual void MousePressCallback(GLFWwindow* window, int button, int action, int mods) = 0;
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	virtual void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) = 0;
	virtual void SetUniformLocations(Shader* shader, bool computeRender = false);
	virtual void SetUniforms(Shader* shader, bool computeRender = false);
	virtual void SetUniformNames();
	virtual void SetVariable(std::string name, std::string value) = 0;
	virtual void HandleKeyInput();
	virtual std::string GetSpecPath(std::string fileName) = 0;
	virtual std::string GetFractalPath(std::string fileName) = 0;
	virtual Shader* GenerateShader(int* specIndex, int* fractalIndex, std::string name) = 0;
	virtual Shader* GenerateShader(std::string name) = 0;
	virtual Shader* GenerateShader() = 0;
	virtual Shader* GenerateShader(int specIndex, int fractalIndex, std::string fractalName) = 0;
	virtual std::string GetFractalFolderPath() = 0;
	virtual void Init();


	static const constexpr char* pathRectangleVertexshader = "shaders/Rectangle.glsl";
	static const constexpr char* rectangleVertexBufferName = "vertices";
	static const constexpr char* rectangleVertexBufferIndexName = "verticesIndex";

	static const constexpr char* shaderSuffixes[] = { "Render", "MapImage", "Specs" };

	// String functions
	static bool Replace(std::string& str, const std::string& from, const std::string& to);
	static bool Replace(std::string& str, const std::string& from, const std::string& to, size_t start);
	static bool ReplaceSection(Section originSection, Section destSection, std::string& origin, std::string& dest);
	static bool ReplaceSection(Section section, std::string& origin, std::string& dest);
	static std::string GetSection(Section s, std::string from, size_t start = 0);
	static std::string GetWholeSection(Section s, std::string from, size_t start = 0);
	static std::string GetSectionName(std::string str);
	static std::string GetSectionValue(std::string str);
	static std::string SubString(std::string str, int begin, int end);
	static void CleanString(std::string& str, std::vector<char> chars);
	static void CleanString(std::string& str, char c);
	static void CleanVector(std::vector<std::string>& str, std::vector<char> chars);
	static bool RemoveOuterSection(std::string& str);
	static bool RemoveOuterChars(std::string& str, char first, char last);
	static std::vector<std::string> Split(std::string str, char splitBy);
	static std::vector<std::string> SplitNotInChar(std::string str, char splitBy, char opener, char closer);
	static std::vector<std::string> SplitNotInChar(std::string str, char splitBy, std::vector<std::pair<char, char>> ignore);
	static std::vector<std::string> GetOuterSections(std::string& source);
	static std::vector<std::string> GetSections(std::string& source);
	static bool StringToBool(std::string str);
	static std::vector<std::string> GetFractalNames(std::vector<std::string> names);
	static std::string GetFractalNames(std::vector<std::string> names, size_t index);
	static bool StringEqualNoCase(const std::string& a, const std::string& b);
	static bool StringContainsNoCase(const std::string& a, const std::string& b);
	static bool VectorContainsNoCase(const std::vector<std::string>& stack, const std::string& needle);
	static bool VectorContainsSubStrNoCase(const std::vector<std::string>& stack, const std::string& needle);
};

#endif