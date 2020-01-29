#include "headers/Fractal.h"
#include "headers/Fractal2D.h"
#include "headers/Fractal3D.h"
#include "headers/Debug.h"
#include "headers/Image.h"
#include "headers/GUI.h"
#include "headers/shader.h"
#include <headers/FileManager.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <thread>
#include <nanogui/nanogui.h>
#include "..\headers\Fractal.h"

#define DoNothing [](){}
#define EmptyThread std::thread(DoNothing)

Uniform<glm::vec2> Fractal::screenSize;
GLFWwindow* Fractal::window;

bool Fractal::renderMode = false;

bool Fractal::Replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

bool Fractal::Replace(std::string& str, const std::string& from, const std::string& to, size_t start)
{
	size_t start_pos = str.find(from, start);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

bool Fractal::ReplaceSection(Section originSection, Section destSection, std::string& origin, std::string& dest)
{
	int index = origin.find(originSection.start);
	if (index == std::string::npos)
	{
		return false;
	}
	int startOrigin = index + originSection.start.length();
	int endOrigin = origin.find(originSection.end, startOrigin);

	return Replace(dest, destSection.start, origin.substr(startOrigin, endOrigin - startOrigin));
}

bool Fractal::ReplaceSection(Section section, std::string& origin, std::string& dest)
{
	int index = origin.find(section.start);
	if (index == std::string::npos)
	{
		return false;
	}
	int startOrigin = index + section.start.length();
	int endOrigin = origin.find(section.end, startOrigin);

	return Replace(dest, section.start, origin.substr(startOrigin, endOrigin - startOrigin));
}

std::string Fractal::GetSection(Section s, std::string from, size_t start)
{
	int startIndex = from.find(s.start, start);
	int endIndex = from.find(s.end, startIndex);

	if (startIndex == std::string::npos) return "";
	else if (endIndex == std::string::npos)
	{
		std::cout << "Could not find closing tag for section " + s.start << std::endl;
	}

	startIndex += s.start.length();

	return from.substr(startIndex, endIndex - startIndex);
}

std::string Fractal::GetWholeSection(Section s, std::string from, size_t start)
{
	int startIndex = from.find(s.start, start);
	int endIndex = from.find(s.end, startIndex);
	if (startIndex == std::string::npos || endIndex == std::string::npos)
	{
		return "";
	}

	return from.substr(startIndex, endIndex - startIndex);
}

std::string Fractal::GetSectionName(std::string str)
{
	int startIndex = str.find('<') + 1;
	int endIndex = str.find('>', startIndex);
	return str.substr(startIndex, endIndex - startIndex);
}

std::string Fractal::GetSectionValue(std::string str)
{
	int startIndex = str.find('>') + 1;
	int endIndex = str.find('<', startIndex);
	return str.substr(startIndex, endIndex - startIndex);
}

void Fractal::CleanString(std::string& str, std::vector<char> chars)
{
	for (size_t i = 0; i < chars.size(); i++)
	{
		str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
	}
}

void Fractal::CleanVector(std::vector<std::string>& str, std::vector<char> chars)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		for (size_t j = 0; j < chars.size(); j++)
		{
			str[i].erase(std::remove(str[i].begin(), str[i].end(), chars[j]), str[i].end());
		}
	}
}

bool Fractal::RemoveOuterSection(std::string& str)
{
	int sectionStart = str.find('<');
	int sectionEnd = str.find('>', sectionStart);
	std::string name = str.substr(sectionStart + 1, sectionEnd - sectionStart - 1);

	if (name != "")
	{
		Section s(name);
		Replace(str, s.start, "");
		Replace(str, s.end, "");
		return true;
	}
	else
	{
		return false;
	}
}

bool Fractal::RemoveOuterChars(std::string& str, char first, char last)
{
	size_t firstIndex = str.find(first);
	if (firstIndex == std::string::npos)
	{
		return false;
	}
	str.erase(firstIndex, 1);

	size_t endIndex = str.find_last_of(last);
	if (endIndex == std::string::npos)
	{
		return false;
	}
	str.erase(endIndex, 1);

	return true;
}

std::vector<std::string> Fractal::Split(std::string str, char splitBy)
{
	std::stringstream test(str);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(test, segment, splitBy))
	{
		seglist.push_back(segment);
	}
	return seglist;
}

std::vector<std::string> Fractal::SplitNotInChar(std::string str, char splitBy, char opener, char closer)
{
	std::vector<std::string> result;

	CleanString(str, { '\n','\t' });

	int level = 0;
	size_t lastIndex = 0;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == splitBy && level == 0)
		{
			result.push_back(str.substr(lastIndex + ((lastIndex == 0) ? 0 : 1), i - lastIndex - ((lastIndex == 0) ? 0 : 1)));
			lastIndex = i;
		}
		else if (str[i] == opener) level++;
		else if (str[i] == closer) level--;
	}

	std::string end = str.substr(lastIndex);
	if (end[0] == splitBy) end.erase(0, 1);
	if (!(end.length() == 1 && end[0] == splitBy) && end != "")
	{
		result.push_back(end);
	}

	return result;
}

std::vector<std::string> Fractal::SplitNotInChar(std::string str, char splitBy, std::vector<std::pair<char, char>> ignore)
{
	std::vector<std::string> result;

	CleanString(str, { '\n','\t' });

	int level = 0;
	size_t lastIndex = 0;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == splitBy && level == 0)
		{
			result.push_back(str.substr(lastIndex + ((lastIndex == 0) ? 0 : 1), i - lastIndex - ((lastIndex == 0) ? 0 : 1)));
			lastIndex = i;
		}
		for (size_t j = 0; j < ignore.size(); j++)
		{
			if (ignore[j].first == str[i])
			{
				level++;
				break;
			}
		}
		for (size_t j = 0; j < ignore.size(); j++)
		{
			if (ignore[j].second == str[i])
			{
				level--;
				break;
			}
		}
	}

	std::string end = str.substr(lastIndex);
	if (end[0] == splitBy) end.erase(0, 1);
	if (!(end.length() == 1 && end[0] == splitBy) && end != "")
	{
		result.push_back(end);
	}

	return result;
}

std::string Fractal::GetSpecificationByIndex(const std::string* specification, int* index, const std::string presets)
{
	int n = std::count(specification->begin(), specification->end(), '{');

	// Wrap around if index is out range
	if (*index < 0)* index = n - 1;
	else if (*index > n - 1)* index = 0;

	int bracketCount = 0;
	int bracketLevel = 0;
	int startIndex = 0;
	for (size_t i = 0; i < specification->length(); i++)
	{
		if ((*specification)[i] == '{')
		{
			if (bracketCount == *index)
			{
				startIndex = i + 1;
				break;
			}
			bracketCount++;
			bracketLevel++;
		}
		else if ((*specification)[i] == '}')
		{
			bracketLevel--;
			if (bracketLevel < 0)
			{
				DebugPrint("Invalid format: more closing than opening brackets");
				return "";
			}
		}
	}


	if (startIndex == 0)
	{
		return "";
	}

	int endIndex = specification->find('}', startIndex);

	if (endIndex == std::string::npos)
	{
		return "";
	}

	std::string section = specification->substr(startIndex, endIndex - startIndex);
	if (section.find(Section("include").start) != std::string::npos)
	{
		std::vector<std::string> includes = Split(GetSectionValue(GetSection(Section("include"), section)), ',');
		if (includes.size() != 0)
		{
			for (size_t i = 0; i < includes.size(); i++)
			{
				CleanString(includes[i], { '\n', '\t', ' ' });
				std::string preset = GetSection(Section(includes[i]), presets);
				LinkSpecification(preset, section);
			}
		}
	}
	return section;
}

void Fractal::LinkSpecification(std::string& source, std::string& target)
{
	std::vector<std::string> sections = GetOuterSections(source);

	for (size_t i = 0; i < sections.size(); i++)
	{
		std::string sectionName = GetSectionName(sections[i]);
		if (target.find(sectionName) == std::string::npos)
		{
			target += sections[i];
		}
		else
		{
			RemoveOuterSection(sections[i]);
			std::vector<std::string> innerSections = SplitNotInChar(sections[i], ',', { {'[', ']'}, {'(', ')'} });
			int sectionStart = target.find(sectionName) + sectionName.length();
			for (size_t i = 0; i < innerSections.size(); i++)
			{
				// Don't overwrite
				if (target.find(GetSectionName(innerSections[i])) == std::string::npos)
				{
					target.insert(sectionStart + 1, innerSections[i] + ",");
				}
			}
		}
	}
}

std::vector<std::string> Fractal::GetOuterSections(std::string& source)
{
	std::vector<std::string> sections(0);
	CleanString(source, { '\n', '\t' });

	int sectionStart = -1;
	int sectionEnd = -1;
	std::string sectionName;
	for (size_t i = 0; i < source.length(); i++)
	{
		if (source[i] == '<')
		{
			sectionStart = i;
			sectionEnd = source.find('>', i);
			sectionName = source.substr(i + 1, sectionEnd - i - 1);

			std::string end = "</" + sectionName + ">";
			int sectionCloserIndex = source.find(end, sectionEnd);
			if (sectionCloserIndex != std::string::npos)
			{
				sections.push_back(source.substr(i, sectionCloserIndex + (sectionName.length() + 3) - i));
				i = sectionCloserIndex + sectionName.length();
			}
		}
	}
	return sections;
}

std::vector<std::string> Fractal::GetSections(std::string& source)
{
	std::vector<std::string> sections(0);

	size_t sectionEnd = -1;
	std::string sectionName;
	for (size_t i = 0; i < source.length(); i++)
	{
		if (source[i] == '<')
		{
			sectionEnd = source.find('>', i);
			size_t nextOpener = source.find('<', i + 1);
			if (nextOpener < sectionEnd) // Edge case, for example: if (a < <someValue>)
			{
				i = nextOpener;
			}


			if (sectionEnd != std::string::npos)
			{
				sections.push_back(source.substr(i, sectionEnd - i + 1));
				i = sectionEnd;
			}
		}
	}

	return sections;
}

bool Fractal::StringToBool(std::string str)
{
	if (str.length() == 0)		return false;
	else if (str.length() == 1) return str[0] != '0';
	else if (str == "true")     return true;
	else if (str == "false")	return false;
	else						return false;
}

std::vector<std::string> Fractal::GetFractalNames(std::vector<std::string> names)
{
	std::vector<std::string> finalNames(0);
	if (names.size() == 1)
	{
		return { names[0].substr(0, names[0].find_last_of('.')) };
	}
	const static size_t shaderSuffixesLength = std::extent<decltype(shaderSuffixes)>::value;
	for (size_t i = 0; i < names.size(); i++)
	{
		bool isWithoutSuffix = true;
		for (size_t j = 0; j < shaderSuffixesLength; j++)
		{
			if (names[i].find(shaderSuffixes[j]) != std::string::npos)
			{
				isWithoutSuffix = false;
				break;
			}
		}
		if (isWithoutSuffix)
		{
			size_t fileTypeDelimeter = names[i].find('.');
			if (fileTypeDelimeter != std::string::npos)
			{
				names[i] = names[i].substr(0, fileTypeDelimeter);
			}
			finalNames.push_back(names[i]);
		}
	}
	return finalNames;
}

std::string Fractal::GetFractalNames(std::vector<std::string> names, size_t index)
{
	std::vector<std::string> finalNames(0);
	if (names.size() == 1)
	{
		return { names[0].substr(0, names[0].find_last_of('.')) };
	}
	const static size_t shaderSuffixesLength = std::extent<decltype(shaderSuffixes)>::value;
	for (size_t i = 0; i < names.size(); i++)
	{
		bool isWithoutSuffix = true;
		for (size_t j = 0; j < shaderSuffixesLength; j++)
		{
			if (names[i].find(shaderSuffixes[j]) != std::string::npos)
			{
				isWithoutSuffix = false;
				break;
			}
		}
		if (isWithoutSuffix)
		{
			size_t fileTypeDelimeter = names[i].find('.');
			if (fileTypeDelimeter != std::string::npos)
			{
				names[i] = names[i].substr(0, fileTypeDelimeter);
			}
			finalNames.push_back(names[i]);
		}
	}

	if (index>=finalNames.size())
	{
		index = 0;
	}

	return finalNames[index];
}

bool Fractal::StringEqualNoCase(const std::string& a, const std::string& b)
{
	return std::equal(a.begin(), a.end(),
		b.begin(), b.end(),
		[](char a, char b) {
			return tolower(a) == tolower(b);
		});
}

bool Fractal::StringContainsNoCase(const std::string& a, const std::string& b)
{
	auto it = std::search(
		a.begin(), a.end(),
		b.begin(), b.end(),
		[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	);
	return (it != a.end());
}

bool Fractal::VectorContainsNoCase(const std::vector<std::string>& stack, const std::string& needle)
{
	for (size_t i = 0; i < stack.size(); i++)
	{
		std::string str = stack[i];
		CleanString(str, { ' ' });
		if (StringEqualNoCase(str, needle))
		{
			return true;
		}
	}
	return false;
}

bool Fractal::VectorContainsSubStrNoCase(const std::vector<std::string>& stack, const std::string& needle)
{
	for (size_t i = 0; i < stack.size(); i++)
	{
		std::string str = stack[i];
		CleanString(str, { ' ' });
		if (StringContainsNoCase(str, needle))
		{
			return true;
		}
	}
	return false;
}


void ScrollCallBackDelegate(GLFWwindow* window, double xoffset, double yoffset)
{
	switch (((Fractal*)glfwGetWindowUserPointer(window))->fractalType)
	{
	default:
		DebugPrint("Case default reached in function ScrollCallBack");
		break;
	case FractalType::fractal2D:
		(reinterpret_cast<Fractal2D*>(glfwGetWindowUserPointer(window)))->ScrollCallback(window, xoffset, yoffset);
		break;
	case FractalType::fractal3D:
		(reinterpret_cast<Fractal3D*>(glfwGetWindowUserPointer(window)))->ScrollCallback(window, xoffset, yoffset);
		break;
	}
}

void MouseCallbackDelegate(GLFWwindow* window, double x, double y)
{
	Fractal* fractal = (reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)));
	fractal->gui->cursorPosCallbackEvent(x, y);

	switch (fractal->fractalType)
	{
	default:
		DebugPrint("Case default reached in function MouseCallback");
		break;
	case FractalType::fractal2D:
		(reinterpret_cast<Fractal2D*>(glfwGetWindowUserPointer(window)))->MouseCallback(window, x, y);
		break;
	case FractalType::fractal3D:
		(reinterpret_cast<Fractal3D*>(glfwGetWindowUserPointer(window)))->MouseCallback(window, x, y);
		break;
	}
}

void MousePressCallbackDelegate(GLFWwindow* window, int button, int action, int mods)
{
	Fractal* fractal = (reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)));
	fractal->gui->mouseButtonCallbackEvent(button, action, mods);

	switch (fractal->fractalType)
	{
	default:
		DebugPrint("Case default reached in function MousePressCallback");
		break;
	case FractalType::fractal2D:
		(reinterpret_cast<Fractal2D*>(glfwGetWindowUserPointer(window)))->MousePressCallback(window, button, action, mods);
		break;
	case FractalType::fractal3D:
		(reinterpret_cast<Fractal3D*>(glfwGetWindowUserPointer(window)))->MousePressCallback(window, button, action, mods);
		break;
	}
}

void FrameBufferSizeCallbackDelegate(GLFWwindow* window, int width, int height)
{
	switch ((reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)))->fractalType)
	{
	default:
		DebugPrint("Case default reached in function FrameBufferSizeCallback");
		break;
	case FractalType::fractal2D:
		(reinterpret_cast<Fractal2D*>(glfwGetWindowUserPointer(window)))->FramebufferSizeCallback(window, width, height);
		break;
	case FractalType::fractal3D:
		(reinterpret_cast<Fractal3D*>(glfwGetWindowUserPointer(window)))->FramebufferSizeCallback(window, width, height);
		break;
	}
}

void CharPressCallback(GLFWwindow* window, unsigned int codepoint)
{
	Fractal* fractal = (reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)));
	fractal->gui->charCallbackEvent(codepoint);
}

void KeyCallbackDelegate(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Fractal* fractal = reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window));
	fractal->gui->keyCallbackEvent(key, scancode, action, mods);

	if (action == GLFW_PRESS && (mods & GLFW_MOD_ALT) == GLFW_MOD_ALT)
	{
		bool update = true;
		switch (key)
		{
		default:
			update = false;
			break;
		case GLFW_KEY_Q:
			fractal->specIndex++;
			break;
		case GLFW_KEY_A:
			fractal->specIndex--;
			break;

		case GLFW_KEY_W:
			fractal->fractalIndex++;
			break;
		case GLFW_KEY_S:
			fractal->fractalIndex--;
			break;

		case GLFW_KEY_E:
			fractal->fractalNameIndex++;
			fractal->SetFractalNameFromIndex(&fractal->fractalNameIndex, fractal->GetFractalFolderPath());
			break;
		case GLFW_KEY_D:
			fractal->fractalNameIndex--;
			fractal->SetFractalNameFromIndex(&fractal->fractalNameIndex, fractal->GetFractalFolderPath());
			break;

		case GLFW_KEY_F:
		case GLFW_KEY_R:
			if (fractal->fractalType == FractalType::fractal2D)
			{
				glfwSetWindowUserPointer(window, new Fractal3D(fractal->specIndex, fractal->fractalIndex, fractal->fractalNameIndex, fractal->screenSize.value));
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else if (fractal->fractalType == FractalType::fractal3D)
			{
				glfwSetWindowUserPointer(window, new Fractal2D(fractal->specIndex, fractal->fractalIndex, fractal->fractalNameIndex, fractal->screenSize.value));
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			delete fractal;
			fractal = (Fractal*)glfwGetWindowUserPointer(window);
			update = true;
			break;
		}
		if (update)
		{
			fractal->UpdateFractalShader();
			return;
		}
	}
	switch (((Fractal*)glfwGetWindowUserPointer(window))->fractalType)
	{
	default:
		DebugPrint("Case default reached in function KeyCallback");
		break;
	case FractalType::fractal2D:
		(reinterpret_cast<Fractal2D*>(glfwGetWindowUserPointer(window)))->KeyCallback(window, key, scancode, action, mods);
		break;
	case FractalType::fractal3D:
		(reinterpret_cast<Fractal3D*>(glfwGetWindowUserPointer(window)))->KeyCallback(window, key, scancode, action, mods);
		break;
	}
}

Fractal::~Fractal()
{
	delete shader;
}

Fractal::Fractal(Shader* shader, Uniform<glm::vec2> screenSize, Time t, std::map<std::string, int*> shaderIndices, float zoom, FractalType f, int fractalIndex,
	int specIndex, int fractalNameIndex, std::string fractalName)
	: shader(shader), zoom(zoom), fractalType(f), time(t, "time", glGetUniformLocation(shader->id, "time")), deltaTime(0, "deltaTime", glGetUniformLocation(shader->id, "deltaTime")),
	fractalIndex(fractalIndex), specIndex(specIndex), fractalName(fractalName), fractalNameIndex(fractalNameIndex), shaderIndices(shaderIndices), holdingMouse(false), fractalUniforms(),
	fractalSourceCode((fractalSourceCode == "") ? "" : fractalSourceCode), subMenus()
{
	Fractal::screenSize = screenSize;
	this->gui = new GUI(window, this);
}

void Fractal::RenderLoop(GLFWwindow* window, Fractal* fractal)
{
	glfwSetCursorPosCallback(window, MouseCallbackDelegate);
	glfwSetMouseButtonCallback(window, MousePressCallbackDelegate);
	glfwSetKeyCallback(window, KeyCallbackDelegate);
	glfwSetCharCallback(window, CharPressCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallbackDelegate);
	glfwSetScrollCallback(window, ScrollCallBackDelegate);

	glViewport(0, 0, int(fractal->screenSize.value.x), int(fractal->screenSize.value.y));

	if (fractal->fractalType == FractalType::fractal3D)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	fractal->shader->Use();
	GlErrorCheck();

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		fractal->Update();

#if _DEBUG
		// Set the window title to our fps
		std::string title = std::to_string(1 / fractal->time.value.GetDeltaTime());
		glfwSetWindowTitle(window, title.c_str());
#endif

		fractal->shader->Use();
		if (fractal->shader->type == ShaderType::fragment)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractal->shader->buffers[Fractal::rectangleVertexBufferIndexName].id);
			glBindVertexArray(fractal->shader->buffers[Fractal::rectangleVertexBufferName].id);
			// rendering, we use ray marching inside the fragment shader
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		else if (fractal->shader->type == ShaderType::compute)
		{
			ComputeShader* compute = reinterpret_cast<ComputeShader*>(fractal->shader);
			compute->Invoke(fractal->screenSize.value);
			if (fractal->frame.value % compute->renderingFrequency == 0)
			{
				reinterpret_cast<Fractal2D*>(fractal)->RenderComputeShader();
			}
		}
		fractal->gui->drawContents();
		fractal->gui->drawWidgets();
		fractal->shader->Use();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
		fractal = (Fractal*)glfwGetWindowUserPointer(window);
		fractal->HandleKeyInput();
	}
}

void Fractal::GenerateSingleImage(GLFWwindow* window, Fractal* fractal)
{
	const double dt = 0.3;
	const double pi2 = 6.28318530717958647692528676655;
	const int imageCount = int(pi2 / dt) + 1;
	const size_t pixelCount = int(fractal->screenSize.value.x * fractal->screenSize.value.y);


	//fractal->zoom.value = 0.0150609445;
	//(reinterpret_cast<Fractal2D*>(fractal))->position.value.x = -0.73962032;
	//(reinterpret_cast<Fractal2D*>(fractal))->position.value.y = 0.210820600;
	fractal->zoom.value = 2.16f;
	(reinterpret_cast<Fractal2D*>(fractal))->position.value.x = -0.18303f;
	(reinterpret_cast<Fractal2D*>(fractal))->position.value.y = 0.f;
	*fractal->shaderIndices["loopExtraOperations"] = 2;
	*fractal->shaderIndices["loopReturn"] = 1;
	fractal->UpdateFractalShader();

	fractal->shader->Use();
	fractal->SetUniformLocations(fractal->shader);
	fractal->SetUniforms(fractal->shader);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractal->shader->buffers[Fractal::rectangleVertexBufferIndexName].id);
	glBindVertexArray(fractal->shader->buffers[Fractal::rectangleVertexBufferName].id);


	std::vector<glm::ivec4> images = std::vector<glm::ivec4>(pixelCount);
	std::vector<Pixel> currentImage = std::vector<Pixel>(pixelCount);

	GlErrorCheck();
	for (double time = 0; time < pi2; time += dt)
	{
		fractal->time.value.SetTotalTime(time);
		fractal->shader->SetUniform(fractal->time);


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glReadPixels(0, 0, int(fractal->screenSize.value.x), int(fractal->screenSize.value.y), GL_RGBA, GL_UNSIGNED_BYTE, &currentImage[0]);


		for (size_t i = 0; i < pixelCount; i++)
		{
			images[i] += (glm::ivec4)currentImage[i];
		}
	}
	GlErrorCheck();

	for (size_t i = 0; i < pixelCount; i++)
	{
		images[i] /= imageCount;
	}

	Image image((int)fractal->screenSize.value.x, (int)fractal->screenSize.value.y, images);
	image.FlipVertically();

	try
	{
		const static std::string baseName = "TestImage/image";
		int count = 0;
		while (FileManager::FileExists(baseName + std::to_string(count) + ".png")) count++;

		image.Save((baseName + std::to_string(count) + ".png").c_str());
	}
	catch (const std::exception& e)
	{
		DebugPrint("Error saving image: " + *e.what());
	}
}

void Fractal::ImageSequence(GLFWwindow* window, Fractal* fractal)
{
	const static std::string baseName = "TestImage/image";
	const static int standardWork = 1920 * 1080;
	const static int imageCount = 100; 
	const static double pi2 = 6.28318530717958647692528676655;
	const static double dt = pi2 / double(imageCount-1); // Don't worry about the -1, you will get x images
	const static int standardFPI = 5; // FPI: frames per image
	const size_t framesPerImage = std::max(standardFPI, int(float(standardFPI) / (float((fractal->screenSize.value.x * fractal->screenSize.value.y)) / standardWork)));

	const size_t pixelCount = int(fractal->screenSize.value.x * fractal->screenSize.value.y);

	glfwSetCursorPosCallback(window, MouseCallbackDelegate);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallbackDelegate);
	glfwSetKeyCallback(window, KeyCallbackDelegate);
	glfwSetScrollCallback(window, ScrollCallBackDelegate);

	glViewport(0, 0, int(fractal->screenSize.value.x), int(fractal->screenSize.value.y));

	if (fractal->fractalType == FractalType::fractal3D)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	fractal->shader->Use();
	GlErrorCheck();

	int count = 0;
	// Find the current lowest count
	while (FileManager::FileExists((baseName + std::to_string(count) + ".png"))) count++;

	std::vector<Pixel> data = std::vector<Pixel>(pixelCount);

	// Ping pong buffering with pbos to render quicker
	unsigned int pboIds[2];
	glGenBuffers(2, pboIds);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, int(screenSize.value.x * screenSize.value.y * 4), 0, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[1]);
	glBufferData(GL_PIXEL_PACK_BUFFER, int(screenSize.value.x * screenSize.value.y * 4), 0, GL_STREAM_READ);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	// Set dt
	fractal->deltaTime.value = (float)dt;
	fractal->shader->SetUniform(fractal->deltaTime);

	// render loop
	double imageRenderTime = glfwGetTime();
	std::thread imageSaveThread(DoNothing);
	Image image(int(screenSize.value.x), int(screenSize.value.y), &data);

	for (size_t i = 0; i < imageCount; i++)
	{
		if (fractal->shader->type == ShaderType::compute)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ((ComputeShader*)fractal->shader)->mainBuffer.id);
			glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F, GL_RED, GL_FLOAT, nullptr);
		}

		fractal->shader->Use();
		fractal->frame.value = 0;
		fractal->shader->SetUniform(fractal->frame);
		fractal->shader->SetUniform(fractal->time.id, float(i*dt));


#if _DEBUG
		// Set the window title to our fps
		std::string title = std::to_string(glfwGetTime() - imageRenderTime);
		glfwSetWindowTitle(window, title.c_str());
#endif
		imageRenderTime = glfwGetTime();
		for (size_t j = 0; j < framesPerImage; j++)
		{
			if (fractal->shader->type == ShaderType::fragment)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractal->shader->buffers[Fractal::rectangleVertexBufferIndexName].id);
				glBindVertexArray(fractal->shader->buffers[Fractal::rectangleVertexBufferName].id);
				// rendering, we use ray marching inside the fragment shader
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			else if (fractal->shader->type == ShaderType::compute)
			{
				reinterpret_cast<ComputeShader*>(fractal->shader)->Invoke(fractal->screenSize.value);
			}
			fractal->frame.value++;
			fractal->shader->SetUniform(fractal->frame);
		}

		if (fractal->shader->type == ShaderType::fragment)
		{
			fractal->FindPathAndSaveImage();
		}
		else if (fractal->shader->type == ShaderType::compute)
		{
			// Ping pong buffering
			static int index = 0;
			int nextIndex = 0;

			// increment current index first then get the next index
			// "index" is used to read pixels from a framebuffer to a PBO
			// "nextIndex" is used to process pixels in the other PBO
			index = (index + 1) % 2;
			nextIndex = (index + 1) % 2;

			fractal->shader->Use();
			fractal->shader->SetUniform(fractal->frame);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			fractal->shader->Use();

			imageSaveThread.join();

			std::string path;
			while (FileManager::FileExists(path = (baseName + std::to_string(count) + ".png"))) count++;

			glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[index]);
			glReadPixels(0, 0, int(screenSize.value.x), int(screenSize.value.y), GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ((ComputeShader*)fractal->shader)->mainBuffer.id);

			// We use ping pong buffers, but still want the first and last
			if (i == 0)
			{
				imageSaveThread = EmptyThread;
				continue;
			}

			glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[nextIndex]);
			Pixel* src = (Pixel*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

			if (src)
			{
				data.assign(src, src + int(screenSize.value.x) * int(screenSize.value.y));
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER);        // release pointer to the mapped buffer

				const static auto Save = [](Image image, std::string path)
				{
					try
					{
						image.FlipAndSave(path);
						DebugPrint("Successfully saved image \"" + FileManager::GetFileName(path) + "\"");
					}
					catch (const std::exception& e)
					{
						DebugPrint("Error saving image: " + *e.what());
						return;
					}
				};
				imageSaveThread = std::thread(Save, image, path);

			}
			// We use ping pong buffers, but still want the first and last
			if (i == imageCount - 1)
			{
				imageSaveThread.join();

				std::string path;
				while (FileManager::FileExists(path = (baseName + std::to_string(count) + ".png"))) count++;

				index = (index + 1) % 2;
				nextIndex = (index + 1) % 2;
				
				glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[nextIndex]);
				Pixel* src = (Pixel*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

				if (src)
				{
					data.assign(src, src + int(screenSize.value.x * screenSize.value.y));
					glUnmapBuffer(GL_PIXEL_PACK_BUFFER);        // release pointer to the mapped buffer

					const static auto Save = [](Image image, std::string path)
					{
						try
						{
							image.FlipAndSave(path);
							DebugPrint("Successfully saved image \"" + FileManager::GetFileName(path) + "\"");
						}
						catch (const std::exception& e)
						{
							DebugPrint("Error saving image: " + *e.what());
							return;
						}
					};
					imageSaveThread = std::thread(Save, image, path);

				}
			}


			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
		fractal = (Fractal*)glfwGetWindowUserPointer(window);
		fractal->HandleKeyInput();
	}

	if (imageSaveThread.joinable()) imageSaveThread.join();
}

void Fractal::SetFractalNameFromIndex(int* index, std::string fractalPath)
{
	std::vector<std::string> fractals = FileManager::GetDirectoryFileNames(fractalPath);
	std::vector<std::string> fractalNames = GetFractalNames(fractals);

	// If index is out range, wrap around
	if (*index < 0)* index = fractalNames.size() - 1;
	if (*index > (int)fractalNames.size() - 1)* index = 0;
	fractalName = fractalNames[*index];
}

void Fractal::SetVariablesFromSpec(int* index, std::string SpecificationPath, std::string presetPath)
{
	std::string specSection = GetSpecificationByIndex(&FileManager::ReadFile(SpecificationPath), index, FileManager::ReadFile(presetPath));
	std::string variables = GetSection(Section("cpuVariables"), specSection);
	if (variables != "")
	{
		std::vector<std::string> variablesList = SplitNotInChar(variables, ',', '[', ']');
		for (size_t i = 0; i < variablesList.size(); i++)
		{
			std::string value = GetSectionValue(variablesList[i]);

			size_t indexStart = value.find('(');
			if (indexStart != std::string::npos)
			{
				size_t indexEnd = value.find(')', indexStart);
				if (indexEnd != std::string::npos)
				{
					size_t index = std::stoi(value.substr(indexStart + 1, indexEnd - 2));
					if (value[value.length() - 1] == ']' && value[value.length() - 2] == ']')
					{
						value.erase(value.length() - 1);
					}
					std::vector<std::string> values = SplitNotInChar(value.substr(indexEnd + 1), ',', '[', ']');
					if (index > values.size() - 1)
					{
						DebugPrint("Index was too large: " + std::to_string(index) + " at " + GetSectionName(variables));
						BreakIfDebug();
					}
					value = values[index];
				}
			}

			CleanString(value, { '[', ']' });
			SetVariable(GetSectionName(variablesList[i]), value);
		}
	}
}

void Fractal::UpdateFractalShader()
{
	delete this->shader;

	this->shader = GenerateShader();

	// Resetting the content in the GUI
	while (gui->childCount())
	{
		gui->removeChild(gui->childAt(0));
	}
	gui->ClearFocusPath();
	gui->nanoGuiWindow = gui->form->addWindow(Eigen::Vector2i(10, 10), gui->guiWindowName);

	for (auto& guiElement : fractalUniforms)
	{
		guiElement.DeleteUniform();
	}

	renderMode = false;

	fractalUniforms.clear();
	subMenus.clear();

	if (fractalType == FractalType::fractal3D)
	{
		(reinterpret_cast<Fractal3D*> (this))->Init();
	}
	else if (fractalType == FractalType::fractal2D)
	{
		(reinterpret_cast<Fractal2D*> (this))->Init();
	}
	else
	{
		DebugPrint("Case default reached in UpdateFractalShader");
	}
}

bool FindSubStrNoCase(const std::string& strHaystack, const std::string& strNeedle)
{
	auto it = std::search(
		strHaystack.begin(), strHaystack.end(),
		strNeedle.begin(), strNeedle.end(),
		[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	);
	return (it != strHaystack.end());
}

void Fractal::PopulateGuiFromShader()
{
	std::string& source = fractalSourceCode;
	
	// Hint start
	const std::string hintS = "<GuiHint>";

	// Hint end
	const std::string hintE = "</GuiHint>";

	int iterationCount = 0;
	while (true)
	{
		size_t hintStart = source.find(hintS);
		if (hintStart == std::string::npos)	break;

		size_t hintEnd = source.find(hintE);
		if (hintEnd == std::string::npos) break;

		std::string hintContent = source.substr(hintStart, hintEnd - hintStart + hintE.length());

		std::string paramsStr = source.substr(hintStart, hintEnd - hintStart);
		std::vector<std::string> params = SplitNotInChar(paramsStr, ',', { {'(',')'} });

		std::string type = GuiElement::GetElement(params, "Type");
		CleanString(type, { ' ' });

		std::string name = GuiElement::GetElement(params, "Name");
		if (name[0] == ' ') name = name.substr(1);

		if (FindSubStrNoCase(hintContent, "submenu"))
		{
			std::string identifier = GuiElement::GetElement(params, "Identifier");
			if (identifier[0] == ' ') identifier = identifier.substr(1);

			subMenus.push_back(new SubMenu(GuiElement::GetElementFromString(type), name, identifier, this));
		}
		else
		{
			hintStart += hintS.length();

			size_t uniformEnd = source.find(";", hintEnd);
			if (uniformEnd == std::string::npos) break;

			hintEnd += hintE.length();

			std::string uniform = source.substr(hintEnd, uniformEnd - hintEnd);

			// Removing start of comment
			if (uniform[0] == '*') uniform = uniform.substr(1);
			if (uniform[0] == '/') uniform = uniform.substr(1);

			CleanString(uniform, { '\n' });

			std::vector<std::string> uniformParts = SplitNotInChar(uniform, ' ', { {'(',')'} });

			std::string parent = GuiElement::GetElement(params, "Parent");
			CleanString(parent, { ' ' });


			fractalUniforms.push_back(GuiElement(GuiElement::GetElementFromString(type), uniformParts[1], uniformParts[2], name, this, uniformParts[uniformParts.size() - 1], parent, params));
		}


		Replace(source, hintContent, "");

		iterationCount++;
		if (iterationCount > 500)
		{
			std::cout << "Oops, we encountered an infinite loop while constructing gui. Please report this message to the dev. Sorry :(" << std::endl;
			BreakIfDebug();
			break;
		}
	}
}

void Fractal::SetShaderUniforms(bool render)
{
	bool preserveOld = Fractal::renderMode;

	Fractal::renderMode = render;

	zoom.SetShaderValue(Fractal::renderMode);

	for (size_t i = 0; i < this->fractalUniforms.size(); i++)
	{
		if (((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue)
		{
			((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue(Fractal::renderMode);
			GlErrorCheck();
		}
		else
		{
#if _DEBUG

			std::cout << ((UniformSuper*)fractalUniforms[i].uniform)->name;
#endif
		}
	}

	Fractal::renderMode = preserveOld;
}

void Fractal::SetShaderGui(bool render)
{
	bool preserveOld = Fractal::renderMode;

	Fractal::renderMode = render;

	zoom.SetGuiValue();


	for (auto& uni : fractalUniforms)
	{
		((UniformSuper*)uni.uniform)->SetGuiValue();
	}

	Fractal::renderMode = preserveOld;
}

int GetCompositeness(int n)
{
	int count = 0;


	while (n % 2 == 0)
	{
		count++;
		n /= 2;
	}

	int root = int(ceil(sqrt(n)));

	for (int i = 3; i <= root; i += 2)
	{
		if (n % i == 0)
		{
			count++;
			n /= i;
		}
	}

	if (n > 2)
	{
		count++;
	}

	return count;
}

int GetMostCompositeInRange(int n, float deviation)
{
	int lowerBound = int(n - n * deviation);
	int upperBound = int(n + n * deviation);

	std::vector<int> numbers(upperBound - lowerBound);

	for (size_t i = 0; i < numbers.size(); i++)
	{
		numbers[i] = GetCompositeness(lowerBound + i);
	}

	int maxFactors = -1;
	int maxNumber = -1;

	for (size_t i = 0; i < numbers.size(); i++)
	{
		if (numbers[i] > maxFactors)
		{
			maxFactors = numbers[i];
			maxNumber = lowerBound + i;
		}
	}

	return maxNumber;
}

glm::vec2 Fractal::GetMonitorSize()
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glm::ivec2 result = { mode->width, mode->height };

	for (int i = 0; i < result.length(); i++)
	{
		if (GetCompositeness(result[i]) == 1)
		{
			result[i] = GetMostCompositeInRange(result[i], 0.01f);
		}
	}

	return (glm::vec2)result;
}

void Fractal::FindPathAndSaveImage()
{
	const static std::string baseName = "TestImage/image";
	int count = 0;
	// Finding the first unused file with name-pattern imageN.png where n is the number ascending
	while (FileManager::FileExists((baseName + std::to_string(count) + ".png"))) count++;

	SaveImage(baseName + std::to_string(count) + ".png");
}

void Fractal::SaveImage(std::string path)
{
	if (shader->type == ShaderType::compute)
	{
		RenderComputeShader();
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader->buffers[Fractal::rectangleVertexBufferIndexName].id);
		glBindVertexArray(shader->buffers[Fractal::rectangleVertexBufferName].id);
		shader->Use();
		SetShaderUniforms(true);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	std::vector<Pixel> data = std::vector<Pixel>(int(screenSize.value.x * screenSize.value.y));
	glReadPixels(0, 0, int(screenSize.value.x), int(screenSize.value.y), GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);



	SetShaderUniforms(false);

	GlErrorCheck();


	Image image(int(screenSize.value.x), int(screenSize.value.y), &data);

	image.FlipVertically();

	try
	{
		image.Save(path.c_str());
		DebugPrint("Successfully saved image \"" + FileManager::GetFileName(path) + "\"");
	}
	catch (const std::exception & e)
	{
		DebugPrint("Error saving image: " + *e.what());
		return;
	}
}

void Fractal::RenderComputeShader()
{
	((ComputeShader*)shader)->UseRender();

	ComputeShader* explShader = reinterpret_cast<ComputeShader*>(shader);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, explShader->buffers[Fractal::rectangleVertexBufferIndexName].id);
	glBindVertexArray(explShader->buffers[Fractal::rectangleVertexBufferName].id);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, explShader->buffers[computeRenderBufferName].id);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, explShader->buffers[computeRenderBufferName].binding, explShader->mainBuffer.id);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	shader->Use();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, explShader->mainBuffer.id);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, explShader->mainBuffer.binding, explShader->mainBuffer.id);
}

void Fractal::PopulateGUI()
{
	gui->form->setFixedSize({ 80,30 });

	gui->form->addGroup("General variables");

	// Slider size
	float dummySize;
	nanogui::Slider* sliderSize = gui->form->AddSlider("Slider Size", dummySize);


	sliderSize->setRange({ 80,1000 });
	sliderSize->setValue((float)sliderSize->fixedWidth());

	sliderSize->setCallback([this](int value)
		{
			for (size_t i = 0; i < this->fractalUniforms.size(); i++)
			{
				if (this->fractalUniforms[i].element == Element::Slider)
				{
					for (size_t j = 0; j < ((Uniform<float>*)this->fractalUniforms[i].uniform)->guiElements.size(); j++)
					{
						nanogui::Slider* slider = (nanogui::Slider*)((Uniform<float>*)this->fractalUniforms[i].uniform)->guiElements[j];
						slider->setFixedWidth(value);
						slider->setWidth(value);
						slider->parent()->setWidth(value + 50);
					}
				}
			}
			
			this->gui->performLayout();
		});

	// Zoom
	GuiElement zoomElement = GuiElement();
	zoomElement.element = Element::TextBox;
	zoomElement.fractal = this;
	zoomElement.uniform = &zoom;
	fractalUniforms.push_back(zoomElement);

	auto zoomField = gui->form->AddTextBox("Zoom", zoom.value);
	zoomField->setCallback([this](float value)
		{
			zoom.SetValue(value, Fractal::renderMode);
			this->shader->SetUniform(zoom);
		});

	zoom.guiElements = { zoomField };
	zoom.SetGuiValue = [this]() { ((nanogui::detail::FormWidget<float, std::true_type>*)this->zoom.guiElements[0])->setValue(zoom.value); };
	zoom.SetShaderValue = [this](bool renderMode) { this->shader->SetUniform(zoom); };

	auto renderCheckbox = gui->form->AddCheckbox("Render Mode", renderMode);
	renderCheckbox->setCallback([this](bool value)
		{
			this->renderMode = value;
			this->SetShaderGui(value);
		});

	// Time
	double dummy;
	auto timeField = gui->form->AddTextBox("Time", dummy);
	timeField->setCallback([this](double value)
		{
			value = std::max(value, 0.);
			this->time.value.SetTotalTime(value);
			this->time.renderValue.SetTotalTime(value);
			this->shader->SetUniform(this->time);
		});

	time.guiElements = { timeField };
	time.SetGuiValue = [this]() { ((nanogui::detail::FormWidget<double, std::true_type>*)this->time.guiElements[0])->setValue(this->time.value.GetTotalTime()); };


	// ParameterChangeRate
	Uniform<float>* changeRate = new Uniform<float>();

	nanogui::Slider* paramSlider = gui->form->AddSlider("parameterChangeRate", parameterChangeRate);
	changeRate->SetGuiValue = [paramSlider, this]() { ((nanogui::Slider*)paramSlider)->setValue(this->parameterChangeRate); };

	paramSlider->setCallback([this](float value) {this->parameterChangeRate = value; });

	paramSlider->setValue(parameterChangeRate);
	paramSlider->setRange({ 0.000001f,5.f });
	changeRate->guiElements = { paramSlider };


	fractalUniforms.push_back(GuiElement(Element::Slider, changeRate, this));


	// Reset button
	nanogui::Button* resetButton = gui->form->AddButton("Reset variables", "reset");
	resetButton->setCallback([this]()
		{
			for (size_t i = 0; i < this->fractalUniforms.size(); i++)
			{
				UniformSuper* uni = (UniformSuper*)fractalUniforms[i].uniform;
				if (fractalUniforms[i].element == Element::Slider || fractalUniforms[i].element == Element::TextBox)
				{
					if (uni->guiElements.size() == 3)
					{
						((Uniform<glm::vec3>*)fractalUniforms[i].uniform)->Reset();

						if (((UniformSuper*)fractalUniforms[i].uniform)->SetGuiValue) ((UniformSuper*)fractalUniforms[i].uniform)->SetGuiValue();
						if (((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue) ((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue(Fractal::renderMode);
					}
					else
					{
						((Uniform<float>*)fractalUniforms[i].uniform)->Reset();
						if (((UniformSuper*)fractalUniforms[i].uniform)->SetGuiValue) ((UniformSuper*)fractalUniforms[i].uniform)->SetGuiValue();
						if (((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue) ((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue(Fractal::renderMode);
					}
				}
				else if (fractalUniforms[i].element == Element::ColorPicker)
				{
					((Uniform<nanogui::Color>*)fractalUniforms[i].uniform)->Reset();
					if (((UniformSuper*)fractalUniforms[i].uniform)->SetGuiValue) ((UniformSuper*)fractalUniforms[i].uniform)->SetGuiValue();
					if (((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue) ((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue(Fractal::renderMode);
				}
				else if (fractalUniforms[i].element == Element::CheckBox)
				{
					((Uniform<bool>*)fractalUniforms[i].uniform)->Reset();
					if (((UniformSuper*)fractalUniforms[i].uniform)->SetGuiValue) ((UniformSuper*)fractalUniforms[i].uniform)->SetGuiValue();
					if (((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue) ((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue(Fractal::renderMode);
				}
			}
		});

	sliderSize->setCallback([this](int value)
		{
			for (size_t i = 0; i < this->fractalUniforms.size(); i++)
			{
				if (this->fractalUniforms[i].element == Element::Slider)
				{
					for (size_t j = 0; j < ((Uniform<float>*)this->fractalUniforms[i].uniform)->guiElements.size(); j++)
					{
						nanogui::Slider* slider = (nanogui::Slider*)((Uniform<float>*)this->fractalUniforms[i].uniform)->guiElements[j];
						slider->setFixedWidth(value);
						slider->setWidth(value);
						slider->parent()->setWidth(value + 50);
					}
				}
			}

			this->gui->performLayout();
		});


	// Close submenus button
	nanogui::Button* collapseButton = gui->form->AddButton("Collapse submenues", "close");
	collapseButton->setCallback([this]()
		{
			for (size_t i = 0; i < this->subMenus.size(); i++)
			{
				subMenus[i]->window->setVisible(false);
				for (auto& sub : subMenus[i]->children)
				{
					sub->setVisible(false);
				}
			}
		});

	#pragma region ClearBuffers
	int i = 0;
	size_t startPos = 0;
	while (true)
	{
		Section clear = Section("shouldBeCleared");
		size_t clearStart = fractalSourceCode.find(clear.start, startPos);
		if (clearStart != std::string::npos)
		{
			size_t clearEnd = fractalSourceCode.find(clear.end, clearStart);
			std::string info = fractalSourceCode.substr(clearStart + clear.start.size(), clearEnd - (clearStart + clear.start.size()));
			std::vector<std::string> content = SplitNotInChar(info, ',', { {'[',']'} });
			
			startPos = clearEnd + clear.end.size();
			size_t bufferTypeStart = fractalSourceCode.rfind("<bufferType>", startPos);
			size_t bufferTypeEnd = fractalSourceCode.rfind("</bufferType>", startPos);

			if (bufferTypeStart == std::string::npos || bufferTypeEnd == std::string::npos)
			{
				DebugPrint("Buffer could not be located. Giving up");
				BreakIfDebug();
				break;
			}
			bufferTypeStart += std::string("<bufferType>").length();
			std::string bufferName = fractalSourceCode.substr(bufferTypeStart, bufferTypeEnd - bufferTypeStart);

			float* counterPtr = nullptr;
			bool* resetFrame = nullptr;
			std::string name = "";
			for (std::map<std::string, Buffer>::iterator buffer = shader->buffers.begin(); buffer != shader->buffers.end(); ++buffer)
			{
				if (StringEqualNoCase(bufferName, buffer->second.name))
				{
					counterPtr = &buffer->second.timesToClear;
					resetFrame = &buffer->second.resetFrame;
					name = buffer->first;
					break;
				}
			}
			// Mainbuffer in a compute shader is not included in shader's buffer map
			if (shader->type == ShaderType::compute)
			{
				if (StringEqualNoCase(bufferName, ((ComputeShader*)shader)->mainBuffer.name))
				{
					counterPtr = &((ComputeShader*)shader)->mainBuffer.timesToClear;
					resetFrame = &((ComputeShader*)shader)->mainBuffer.resetFrame;
					name = ((ComputeShader*)shader)->mainBuffer.name;
				}
			}

			if (counterPtr == nullptr)
			{
				DebugPrint("Could not find appropriate buffer");
				BreakIfDebug();
				continue;
			}

			bool clearFrame = VectorContainsNoCase(content, "resetFrame");
			if (clearFrame)
			{
				*resetFrame = true;
			}
			
			if (VectorContainsNoCase(content, "checkbox"))
			{
				bool dummy;
				nanogui::CheckBox* checkBox = gui->form->AddCheckbox("Clear " + name, dummy);
				
				checkBox->setCallback([counterPtr](bool checked)
					{
						if (checked)
						{
							*counterPtr = std::numeric_limits<float>::infinity();
						}
						else
						{
							*counterPtr = 0;
						}
					}
				);
			}
			else if (VectorContainsNoCase(content, "button"))
			{
				nanogui::Button* checkBox = gui->form->AddButton("Clear " + name);
				checkBox->setCallback([counterPtr]()
					{
						*counterPtr = 1;
					}
				);
			}

		}
		else
		{
			break;
		}

		// In case the loop turns out infinite
		if (i > 1024)
		{
			BreakIfDebug();
			DebugPrint("The loop turned out infinite :(. Program may suffer a stroke or work completely fine from this point");
		}
	}
#pragma endregion
}

void Fractal::Update()
{
	frame.value++;

	time.value.PollTime();
	time.SetGuiValue();
	deltaTime.value = (float)time.value.GetDeltaTime();


	shader->Use();
	shader->SetUniform(time);
	shader->SetUniform(frame);
	shader->SetUniform(deltaTime);

	// Clearing buffers that should be cleared
	for (std::map<std::string, Buffer>::iterator buffer = shader->buffers.begin(); buffer != shader->buffers.end(); ++buffer)
	{
		if (buffer->second.timesToClear > 0.f)
		{
			buffer->second.timesToClear--;
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->second.id);
			glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F, GL_RED, GL_FLOAT, nullptr);

			if (buffer->second.resetFrame)
			{
				frame.value = 1;
			}
		}
	}
	if (shader->type == ShaderType::compute)
	{
		ComputeShader* compute = (ComputeShader*)shader;
		if (compute->mainBuffer.timesToClear > 0.f)
		{
			compute->mainBuffer.timesToClear--;
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, compute->mainBuffer.id);
			glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F, GL_RED, GL_FLOAT, nullptr);
			if (compute->mainBuffer.resetFrame)
			{
				frame.value = 1;
			}
		}
	}
	if (shader->type == ShaderType::compute)
	{
		((ComputeShader*)shader)->UseRender();
		glUniform1f(((ComputeShader*)shader)->uniformRenderIds[time.name], (float)time.value.GetTotalTime());
		glUniform1ui(((ComputeShader*)shader)->uniformRenderIds[frame.name], frame.value);
		glUniform1f(((ComputeShader*)shader)->uniformRenderIds[deltaTime.name], deltaTime.value);
		shader->Use();
	}
}


void Fractal::BuildMainLoop(Section targetSection, std::string& source, const std::string& defaultSource, std::string& target, std::string& specification, int* index, std::map<std::string, int*> indices)
{
	std::string distanceDeclaration;
	size_t distanceStart = source.find(targetSection.start);
	if (distanceStart == std::string::npos)
	{
		distanceStart = defaultSource.find(targetSection.start);
		if (distanceStart == std::string::npos)
		{
			DebugPrint("Error building distance estimator: could not find distance estimator section");
			return;
		}
		else
		{
			distanceDeclaration = GetSection(targetSection, defaultSource, distanceStart);
		}
	}
	else
	{
		distanceDeclaration = GetSection(targetSection, source, distanceStart);
	}

	if (!Replace(target, targetSection.start, distanceDeclaration))
	{
		DebugPrint("Could not insert distance estimator into target");
		return;
	}
	std::vector<std::string> sourceSections = GetSections(distanceDeclaration);

	sourceSections.erase(std::remove_if(
		sourceSections.begin(), sourceSections.end(),
		[](const std::string& x)
		{
			return x.substr(1, 8) != "distance" && x.substr(1, 4) != "loop";
		}),
		sourceSections.end());

	std::string distanceSpec = GetSection(targetSection, specification);

	std::vector<std::string> distanceSections = SplitNotInChar(distanceSpec, ',', { {'[', ']'}, {'(', ')'} });

	for (size_t k = 0; k < distanceSections.size(); k++)
	{
		CleanString(distanceSections[k], { '\n', '\t', ' ' });
		std::string sectionName = GetSectionName(distanceSections[k]);
		for (size_t i = 0; i < sourceSections.size(); i++)
		{
			if (GetSectionName(sourceSections[i]) == sectionName)
			{
				std::string sectionValue = GetSectionValue(distanceSections[k]);
				std::string final;

				RemoveOuterChars(sectionValue, '[', ']');

				if (sectionValue.find('[') != std::string::npos)
				{
					std::vector<std::string> sequences = SplitNotInChar(sectionValue, ',', '[', ']');

					int* relevantIndex = (indices.count(sectionName)) ? indices[sectionName] : index;


					if (*relevantIndex < 0)*relevantIndex = sequences.size() - 1;
					else if (*relevantIndex > (int)sequences.size() - 1)*relevantIndex = 0;

					sectionValue = sequences[*relevantIndex];
				}

				CleanString(sectionValue, { '[', ']' });
				std::vector<std::string> toReplace = SplitNotInChar(sectionValue, ',', { { '[', ']' }, { '(', ')' } });

				for (size_t j = 0; j < toReplace.size(); j++)
				{
					CleanString(toReplace[j], { '\n', '\t', ' ', '[', ']' });

					Section c("");

					size_t parameterStart = toReplace[j].find('(');
					if (parameterStart != std::string::npos)
					{
						c = toReplace[j].substr(0, parameterStart);
					}
					else
					{
						c = toReplace[j];
					}



					size_t start = source.find(c.start);

					std::string newSection;

					if (start != std::string::npos)
					{
						std::string newSect = GetSection(c, source, start);
						if (newSect == "")
						{
							newSect = GetSection(c, source);
						}
						newSection += newSect;
					}
					else if ((start = defaultSource.find(c.start)) != std::string::npos)
					{
						std::string newSect = GetSection(c, defaultSource, start);
						if (newSect == "")
						{
							newSect = GetSection(c, defaultSource);
						}
						newSection += newSect;
					}
					else
					{
						newSection += "";
					}

					if (parameterStart != std::string::npos)
					{
						size_t parameterEnd = toReplace[j].find_last_of(')');
						if (parameterEnd != std::string::npos)
						{
							parameterStart++;
							std::string parameterValue = toReplace[j].substr(parameterStart, parameterEnd - parameterStart);

							if (parameterValue == "")
							{
								while(Replace(newSection, "parameter", parameterValue)) {}
							}
							else
							{
								std::vector<std::string> parameters = SplitNotInChar(parameterValue, ',', { { '(', ')' } });

								// The first parameter is simply named parameter instead of parameter0
								// This causes conflict with the normal replace function, since it will match with parameter1, parameter2, etc.
								if (parameters.size())
								{
									while (true)
									{
										const size_t paramLength = std::string("parameter").size();
										size_t start = newSection.find("parameter");

										// Prevent breaking cases like "parameter = parameter1 + parameter";

										while (true)
										{
											if (start + paramLength < newSection.size())
											{
												if (newSection[start + paramLength] > 47 && newSection[start + paramLength] < 58)
												{
													size_t newStart = newSection.find("parameter", start + paramLength);

													if (start == newStart || newStart == std::string::npos) goto loopEnd;
													else start = newStart;
												}
												else
												{
													break;
												}
											}
										}

										if (start == std::string::npos) break;

										char parameterPostfix = newSection[start + std::string("parameter").length()];
										if (parameterPostfix < 48 || parameterPostfix > 57) // The postfix isn't a number
										{
											Replace(newSection, "parameter", parameters[0], start);
										}
										else
										{
											break;
										}
									}
								}

								loopEnd:

								if (parameters.size() > 1)
								{
									for (size_t i = 1; i < parameters.size(); i++)
									{
										while (Replace(newSection, "parameter" + std::to_string(i), parameters[i])) {}
									}
								}
							}
						}
					}

					final += newSection;
				}

				while (Replace(target, Section(sectionName).start, final));
			}
		}
	}
}

void Fractal::BuildMainLoop(Section targetSection, std::string& source, const std::string& defaultSource, std::string& target, std::string& specification, std::map<std::string, int*> indices)
{
	int* index = new int(-1);
	BuildMainLoop(targetSection, source, defaultSource, target, specification, index, indices);
	delete index;
}

void Fractal::SetUniformLocations(Shader* shader, bool computeRender)
{
	unsigned int id = (computeRender) ? ((ComputeShader*)shader)->renderId : shader->id;
	if (computeRender) { ((ComputeShader*)shader)->UseRender(); }
	else { shader->Use(); }

	screenSize.id = glGetUniformLocation(id, screenSize.name.c_str());
	frame.id = glGetUniformLocation(id, frame.name.c_str());
	zoom.id = glGetUniformLocation(id, zoom.name.c_str());
	time.id = glGetUniformLocation(id, time.name.c_str());
}

void Fractal::SetUniforms(Shader* shader, bool computeRender)
{
	if (computeRender) { ((ComputeShader*)shader)->UseRender(); }
	else { shader->Use(); }

	shader->SetUniform(frame);
	shader->SetUniform(zoom);
	shader->SetUniform(time);
	shader->SetUniform(screenSize);
}

void Fractal::SetUniformNames()
{
	screenSize.name = "screenSize";
	frame.name = "frame";
	zoom.name = "zoom";
	time.name = "time";
}

void Fractal::HandleKeyInput()
{
	
}

void Fractal::Init()
{

	int i = 0;
	size_t startPos = 0;
	while (true)
	{
		Section clear = Section("shouldBeCleared");
		size_t clearStart = fractalSourceCode.find(clear.start, startPos);
		if (clearStart != std::string::npos)
		{
			size_t clearEnd = fractalSourceCode.find(clear.end, clearStart);
			std::string info = fractalSourceCode.substr(clearStart + clear.start.size(), clearEnd - (clearStart + clear.start.size()));
			std::vector<std::string> content = SplitNotInChar(info, ',', { {'[',']'} });

			startPos = clearEnd + clear.end.size();
			
			size_t bufferTypeStart = fractalSourceCode.rfind("<bufferType>", startPos);
			size_t bufferTypeEnd = fractalSourceCode.rfind("</bufferType>", startPos);

			if (bufferTypeStart == std::string::npos || bufferTypeEnd == std::string::npos)
			{
				DebugPrint("Buffer could not be located. Giving up");
				BreakIfDebug();
				break;
			}
			bufferTypeStart += std::string("<bufferType>").length();
			std::string bufferName = fractalSourceCode.substr(bufferTypeStart, bufferTypeEnd - bufferTypeStart);

			int id = -1;
			
			for (std::map<std::string, Buffer>::iterator buffer = shader->buffers.begin(); buffer != shader->buffers.end(); ++buffer)
			{
				if (StringEqualNoCase(bufferName, buffer->second.name))
				{
					id = buffer->second.id;
					break;
				}
			}

			if (shader->type == ShaderType::compute && id == -1)
			{
				id = ((ComputeShader*)shader)->mainBuffer.id;
			}

			if (id == -1)
			{
				DebugPrint("Could not find appropriate buffer id");
				BreakIfDebug();
				return;
			}


			if (VectorContainsSubStrNoCase(content, "onUniformChange"))
			{
				std::string onChangeStr = "";

				for (size_t i = 0; i < content.size(); i++)
				{
					if (StringContainsNoCase(content[i], "onUniformChange"))
					{
						onChangeStr = content[i];
						break;
					}
				}

				if (onChangeStr == "")
				{
					DebugPrint("Something has gone terribly wrong");
					BreakIfDebug();
					return;
				}

				size_t arrayStart = onChangeStr.find('[');
				size_t arrayEnd = onChangeStr.find(']');
				if (arrayStart == std::string::npos || arrayEnd == std::string::npos)
				{
					DebugPrint("Could not locate array location, giving up");
					BreakIfDebug();
					return;
				}
				
				arrayStart += 1;

				std::vector<std::string> onChange = Split(onChangeStr.substr(arrayStart, arrayEnd - arrayStart), ',');
				CleanVector(onChange, { ' ' });

				auto callBack = [id](void* fractal)
				{
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
					glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F, GL_RED, GL_FLOAT, nullptr);
					((Fractal*)fractal)->frame.value = 1;
				};

				for (size_t i = 0; i < fractalUniforms.size(); i++)
				{
					if (VectorContainsNoCase(onChange, fractalUniforms[i].uniform->name))
					{
						fractalUniforms[i].uniform->callbacks.push_back(callBack);
						fractalUniforms[i].uniform->fractal = this;
					}
				}
			}
		}
		else
		{
			break;
		}

		// In case the loop turns out infinite
		if (i > 1024)
		{
			BreakIfDebug();
			DebugPrint("The loop turned out infinite :(. Program may suffer a stroke or work completely fine from this point");
		}
	}
}
