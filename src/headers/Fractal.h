#pragma once
#ifndef FRACTAL_H
#define FRACTAL_H

#include "headers/GUI.h"
#include <GLFW/glfw3.h>
#include <string>

#include "headers/Uniform.h"
#include "headers/Shader.h"
#include "headers/Camera.h"
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
class Camera;

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
	Uniform<Time> time;
	Uniform<float> deltaTime;
	Uniform<unsigned int> frame;
	Camera* camera;
	Uniform<glm::vec2> mousePosition;
	Uniform<glm::vec2> clickPosition;


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
	Fractal(FractalType fractalType, int specIndex, int fractalIndex, int fractalNameIndex, Uniform<glm::vec2> screenSize);

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
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void HandleKeyInput();
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseCallback(GLFWwindow* window, double x, double y);
	glm::vec2 MapScreenMouseToFractal();
	void MousePressCallback(GLFWwindow* window, int button, int action, int mods);
	void PopulateGUI();
	void Init();
	void Update();
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	void SetUniformLocations(Shader* shader, bool computeRender = false);
	void SetUniforms(Shader* shader, bool computeRender = false);
	void SetUniformNames();
	void SetVariable(std::string name, std::string value);
	std::vector<int> GetPrimeFactors(int n);
	void ParseShader(std::string& source, std::string & final, const std::string* spec, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections);
	void ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string & final, std::string specification);
	Shader* CreateShader(std::string source, const std::string* specification, int* fractalIndex, int* specIndex, std::vector<ShaderSection> shaderSections);

	std::string GetDefaultFractalSourcePath();
	std::string GetAlternateDefaultFunctionsPath();
	std::string GetPresetSpecPath();
	std::string GetHelperFunctionPath();
	std::string GetBasePath();
	std::string GetComputeBasePath();
	std::string GetFractalPath();
	std::string GetFractalFolderPath();
	std::string GetSpecPath(std::string fileName);
	std::string GetFractalPath(std::string fileName);
	std::vector<ShaderSection> GetPostShaderSections();
	std::vector<ShaderSection> GetShaderSections();

	Shader* GenerateShader(int* specIndex, int* fractalIndex, std::string name);
	Shader* GenerateShader(int specIndex, int fractalIndex, std::string fractalName);
	Shader* GenerateShader(std::string name);
	Shader* GenerateShader();


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
	static std::string Trim(std::string str);
	static std::string SubString(std::string str, size_t start, size_t end);
	static void RemoveFirstCharIfEqual(std::string& str, char c = ' ');
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
	static std::vector<std::string> GetCharsWithinDelimeters(std::string content, std::string open, std::string close);
	static bool StringToBool(std::string str);
	static std::vector<std::string> GetFractalNames(std::vector<std::string> names);
	static std::string GetFractalNames(std::vector<std::string> names, size_t index);
	static bool StringEqualNoCase(const std::string& a, const std::string& b);
	static bool StringContainsNoCase(const std::string& a, const std::string& b);
	static bool VectorContainsNoCase(const std::vector<std::string>& stack, const std::string& needle);
	static bool VectorContainsSubStrNoCase(const std::vector<std::string>& stack, const std::string& needle);
};

#endif