#include "headers/Fractal.h"
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

#define PrintSource 1

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
	if (endOrigin == std::string::npos)
	{
		DebugPrint("Could not find closing tag " + originSection.end);
		BreakIfDebug();
	}

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
		return "";
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

std::string Fractal::SubString(std::string str, int begin, int end)
{
	return str.substr(begin, end-begin);
}

std::string Fractal::Trim(std::string str)
{
	std::vector<char> charsToTrim = { ' ', '	' };
	// Leading whitespace
	for (size_t i = 0; i < str.size(); i++)
	{
		bool isWhiteSpace = false;
		for (size_t j = 0; j < charsToTrim.size(); j++)
		{
			if (str[i] == charsToTrim[j])
			{
				isWhiteSpace = true;
				break;
			}
		}
		if (isWhiteSpace)
		{
			i--;
			str = str.substr(1);
		}
		else
		{
			break;
		}
	}

	// Trailing whitespace
	for (int i = str.size() - 1; i >= 0; i--)
	{
		bool isWhiteSpace = false;
		for (size_t j = 0; j < charsToTrim.size(); j++)
		{
			if (str[i] == charsToTrim[j])
			{
				isWhiteSpace = true;
				break;
			}
		}
		if (isWhiteSpace)
		{
			// Do not increment i, as it is done "automatically" by making the size of the string smaller
			str = str.substr(0, str.size()-1);
		}
		else
		{
			break;
		}
	}
	return str;
}

std::string Fractal::SubString(std::string str, size_t start, size_t end)
{
	return str.substr(start, end - start);
}

void Fractal::RemoveFirstCharIfEqual(std::string& str, char c)
{
	if (str[0] == c) str = str.substr(1);
}

void Fractal::CleanString(std::string& str, std::vector<char> chars)
{
	for (size_t i = 0; i < chars.size(); i++)
	{
		str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
	}
}

void Fractal::CleanString(std::string& str, char c)
{
	str.erase(std::remove(str.begin(), str.end(), c), str.end());
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

		CleanVector(includes, { '\n', '\t', ' ' });

		for (size_t i = 0; i < includes.size(); i++)
		{
			CleanString(includes[i], { '\n', '\t', ' ' });
			std::string preset = GetSection(Section(includes[i]), presets);
			LinkSpecification(preset, section);
		}

		std::vector<std::string> newIncludes;
		int i = 0;
		std::vector<std::string> notDone;
		do
		{
			notDone.clear();
			newIncludes = Split(GetSectionValue(GetSection(Section("include"), section)), ',');
			
			if (newIncludes.size() != includes.size())
			{
				// Find the ones that have not been processed
				CleanVector(newIncludes, { '\n', '\t', ' ' });
				for (size_t i = 0; i < newIncludes.size(); i++)
				{
					if (std::find(includes.begin(), includes.end(), newIncludes[i]) == includes.end())
					{
						notDone.push_back(newIncludes[i]);
						includes.push_back(newIncludes[i]);
					}
				}


				for (size_t i = 0; i < notDone.size(); i++)
				{
					CleanString(notDone[i], { '\n', '\t', ' ' });
					std::string preset = GetSection(Section(notDone[i]), presets);
					LinkSpecification(preset, section);
				}
			}

			i++;
			// Break if infinite loop
			if (i > 512)
			{
				DebugPrint("Infinite loop, sorry");
				BreakIfDebug();
				break;
			}
		} while (notDone.size());
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

std::vector<std::string> Fractal::GetCharsWithinDelimeters(std::string content, std::string open, std::string close)
{
	std::vector<std::string> strings;

	int level = 0;
	std::string current = "";

	try
	{
		for (size_t i = 0; i < content.size(); i++)
		{
			if (content[i] == open[0])
			{
				bool same = true;
				for (size_t j = 0; j < open.size(); j++)
				{
					if (content[i + j] != open[j])
					{
						same = false;
						break;
					}
				}
				if (same)
				{
					level++;
					i += open.size();
				}
			}
			else if (content[i] == close[0])
			{
				bool same = true;
				for (size_t j = 0; j < close.size(); j++)
				{
					if (content[i + j] != close[j])
					{
						same = false;
						break;
					}
				}
				if (same)
				{
					level--;
					i += close.size();
					if (level == 0)
					{
						strings.push_back(current);
						current = "";
					}
				}
			}
			if (level > 0)
			{
				current += content[i];
			}
		}		
	}
	catch (const std::exception & ex)
	{
		return {};
	}
	return strings;
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


void ScrollCallBackDelegate(GLFWwindow* window, double xOffset, double yOffset)
{
	(reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)))->ScrollCallback(window, xOffset, yOffset);
}

void MouseCallbackDelegate(GLFWwindow* window, double x, double y)
{
	Fractal* fractal = (reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)));
	fractal->gui->cursorPosCallbackEvent(x, y);
	fractal->MouseCallback(window, x, y);
}

void MousePressCallbackDelegate(GLFWwindow* window, int button, int action, int mods)
{
	Fractal* fractal = (reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)));
	fractal->gui->mouseButtonCallbackEvent(button, action, mods);
	fractal->MousePressCallback(window, button, action, mods);
}

void FrameBufferSizeCallbackDelegate(GLFWwindow* window, int width, int height)
{
	(reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)))->FramebufferSizeCallback(window, width, height);
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

	fractal->KeyCallback(window, key, scancode, action, mods);


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
			fractal->SetFractalNameFromIndex(&fractal->fractalNameIndex, fractal->GetFractalPath());
			break;
		case GLFW_KEY_D:
			fractal->fractalNameIndex--;
			fractal->SetFractalNameFromIndex(&fractal->fractalNameIndex, fractal->GetFractalPath());
			break;

		case GLFW_KEY_F:
		case GLFW_KEY_R:
			// Invert fractal type
			FractalType newFractalType = (fractal->fractalType == FractalType::fractal2D) ? FractalType::fractal3D : FractalType::fractal2D;
			glfwSetWindowUserPointer(window, new Fractal(newFractalType, fractal->specIndex, fractal->fractalIndex, fractal->fractalNameIndex, fractal->screenSize.value));
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			
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
}

Fractal::~Fractal()
{
	delete shader;
}

Fractal::Fractal(FractalType fractalType, int specIndex, int fractalIndex, int fractalNameIndex, Uniform<glm::vec2> screenSize)
	: shader(), fractalType(fractalType), time(), deltaTime(0.05f), specIndex(specIndex), fractalIndex(fractalIndex), fractalNameIndex(fractalNameIndex), fractalName(GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalPath()), fractalNameIndex)),
	shaderIndices((shaderIndices.size() == 0) ? std::map<std::string, ShaderIndice*>() : shaderIndices), holdingMouse(false), fractalUniforms(),
	fractalSourceCode((fractalSourceCode == "") ? "" : fractalSourceCode), subMenus(), camera((fractalType == FractalType::fractal3D) ? DefaultCamera3D : DefaultCamera2D),
	mousePosition(), clickPosition()
{
	Fractal::screenSize = screenSize;
	this->gui = new GUI(window, this);
	
	shader = GenerateShader(specIndex, fractalIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalPath()), fractalNameIndex));
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
				fractal->RenderComputeShader();
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
	fractal->camera->zoom.value = 2.16f;
	fractal->camera->position.value.x = -0.18303f;
	fractal->camera->position.value.y = 0.f;
	*fractal->shaderIndices["loopExtraOperations"]->value = 2;
	*fractal->shaderIndices["loopReturn"]->value = 1;
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
	const static std::string baseName = "TestImage/Animation/image";
	const static int standardWork = 1920 * 1080;
	const static int imageCount = 150; 
	const static double pi2 = 6.28318530717958647692528676655;
	const static double dt = pi2 / double(imageCount-1); // Don't worry about the -1, you will get x images
	const static int standardFPI = 40; // FPI: frames per image
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

	// Set variables
	fractal->shaderIndices["loopSetup"]->value = new int(3);
	fractal->shaderIndices["loopExtraOperations"]->value = new int(1);
	//fractal->shaderIndices["loopSetup"]->value = new int(3);
	fractal->UpdateFractalShader();

	((ComputeShader*)fractal->shader)->UseRender();
	((Uniform<float>*)fractal->fractalUniforms[6].uniform)->value = 0.516667f;
	((Uniform<float>*)fractal->fractalUniforms[6].uniform)->SetShaderValue(false);
	((Uniform<float>*)fractal->fractalUniforms[4].uniform)->value = 770.25;
	((Uniform<float>*)fractal->fractalUniforms[4].uniform)->SetShaderValue(false);
	fractal->shader->Use();
	((Uniform<bool>*)fractal->fractalUniforms[17].uniform)->value = false;
	((Uniform<bool>*)fractal->fractalUniforms[17].uniform)->SetShaderValue(false);
	((Uniform<glm::vec3>*)fractal->fractalUniforms[21].uniform)->value = { 0.0 ,0.2,1 };
	((Uniform<glm::vec3>*)fractal->fractalUniforms[21].uniform)->SetShaderValue(false);
	fractal->clickPosition.value = { -0.0263000727, - 0.629090190  }; // -0.753948450, -0.0111111403
	fractal->shader->SetUniform(fractal->clickPosition);
	fractal->camera->position.value = { 0.563757, 0.437172, 0 };
	fractal->shader->SetUniform(fractal->camera->position);
	fractal->camera->zoom.value = 0.142779f;
	fractal->shader->SetUniform(fractal->camera->zoom);

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
		//fractal->shader->SetUniform(fractal->time.id, float(i*dt));
		fractal->shader->SetUniform(fractal->time.id, 24.0962f);
		((Uniform<glm::vec3>*)fractal->fractalUniforms[21].uniform)->value = { 0.142857, float(i)/float(imageCount), 1};
		((Uniform<glm::vec3>*)fractal->fractalUniforms[21].uniform)->SetShaderValue(false);


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

			((ComputeShader*)fractal->shader)->UseRender();
			((ComputeShader*)fractal->shader)->SetUniform(((ComputeShader*)fractal->shader)->uniformRenderIds["frame"], fractal->frame.value);

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
	// Resetting uniform callbacks
	clickPosition.callbacks = {};
	screenSize.callbacks = {};
	time.callbacks = {};
	deltaTime.callbacks = {};
	frame.callbacks = {};
	mousePosition.callbacks = {};
	camera->GetRotationMatrix().callbacks = {};
	camera->viewMode3D.callbacks = {};
	camera->position.callbacks = {};
	camera->zoom.callbacks = {};

	subMenus.clear();

	camera->cursorVisible = true;
	double x;
	double y;
	glfwGetCursorPos(window, &x, &y);
	camera->lastNonGuiPos = { x,y };
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	Init();
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

			std::string parentIdentifier = GuiElement::GetElement(params, "Parent");
			SubMenu* parent = nullptr;
			Eigen::Vector2i offset = { 0,0 };
			if (parentIdentifier != "")
			{
				offset = { 130, -10 };
				RemoveFirstCharIfEqual(parentIdentifier);

				for (size_t i = 0; i < subMenus.size(); i++)
				{
					if (StringEqualNoCase(subMenus[i]->identifier, parentIdentifier))
					{
						parent = subMenus[i];
					}
				}

				if (parent == nullptr)
				{
					DebugPrint("Could not find parent, will do a default");
					BreakIfDebug();
					parent = subMenus[0];
				}
			}

			subMenus.push_back(new SubMenu(GuiElement::GetElementFromString(type), name, identifier, this, offset, parent));
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

	camera->zoom.SetShaderValue(Fractal::renderMode);

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


	for (auto& uni : fractalUniforms)
	{
		if (((UniformSuper*)uni.uniform)->SetGuiValue)
		{
			((UniformSuper*)uni.uniform)->SetGuiValue();
		}
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

	SubMenu* cameraMenu = new SubMenu(Element::SubMenu, "Camera", "cam", this, { 65, 0 });
	subMenus.push_back(cameraMenu);
	camera->cameraMenu = cameraMenu;

	cameraMenu->form->setFixedSize({ 75, 25 });

	camera->useBuddhabrotRotation = shader->type == ShaderType::compute;
	camera->viewMode3D.value = fractalType == FractalType::fractal3D;
	camera->PopulateCameraGUI(this);

	// Render mode
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
	changeRate->name = "changeRate";

	nanogui::Slider* paramSlider = gui->form->AddSlider("parameterChangeRate", parameterChangeRate);
	changeRate->SetGuiValue = [paramSlider, this]() { ((nanogui::Slider*)paramSlider)->setValue(this->parameterChangeRate); };

	paramSlider->setCallback([this](float value) {this->parameterChangeRate = value; });

	paramSlider->setValue(parameterChangeRate);
	paramSlider->setRange({ 0.000001f,5.f });
	changeRate->guiElements = { paramSlider };
	changeRate->PrintValue = [this]() {std::cout << "ParameterChangeRate: " << this->parameterChangeRate << std::endl; };


	fractalUniforms.push_back(GuiElement(Element::Slider, changeRate, this));


	SubMenu* debugMenu = new SubMenu(Element::SubMenu, "Debug", "debug", this, { 65, 0 });
	subMenus.push_back(debugMenu);

	debugMenu->form->setFixedSize({ 75, 25 });

	nanogui::Button* button = debugMenu->form->AddButton("Print uniform values");
	button->setCallback([this]()
		{
			for (size_t i = 0; i < this->fractalUniforms.size(); i++)
			{
				if (this->fractalUniforms[i].uniform->PrintValue)
				{
					this->fractalUniforms[i].uniform->PrintValue();
				}
				
			}
			this->time.PrintValue();
			this->clickPosition.PrintValue();
		});


	// Reset button
	nanogui::Button* resetButton = gui->form->AddButton("Reset variables", "reset");
	resetButton->setCallback([this]()
		{
			for (size_t i = 0; i < this->fractalUniforms.size(); i++)
			{
				UniformSuper* uni = (UniformSuper*)fractalUniforms[i].uniform;
				
				switch (fractalUniforms[i].element)
				{
				default:
					break;

				case Element::Slider:
				case Element::TextBox:
					if (uni->guiElements.size() == 4)	((Uniform<glm::vec4>*)uni)->Reset();
					if (uni->guiElements.size() == 3)	((Uniform<glm::vec3>*)uni)->Reset();
					if (uni->guiElements.size() == 2)	((Uniform<glm::vec2>*)uni)->Reset();
					if (uni->guiElements.size() == 1)		((Uniform<float>*)uni)->Reset();
					break;
				case Element::ColorPicker:
					((Uniform<nanogui::Color>*)fractalUniforms[i].uniform)->Reset();
					break;
				case Element::CheckBox:
					((Uniform<bool>*)fractalUniforms[i].uniform)->Reset();
					break;
				}
			
				if (((UniformSuper*)fractalUniforms[i].uniform)->SetGuiValue) ((UniformSuper*)fractalUniforms[i].uniform)->SetGuiValue();
				if (((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue) ((UniformSuper*)fractalUniforms[i].uniform)->SetShaderValue(Fractal::renderMode);
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

	if (holdingMouse && !camera->viewMode3D.value)
	{
		shader->Use();

		// Map from screen space to fractal space
		clickPosition.SetValue(MapScreenMouseToFractal(), false);
		shader->SetUniform(clickPosition);
	}

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

void Fractal::MouseCallback(GLFWwindow* window, double x, double y)
{
	// If cursor is not set to first person, do not move
	if (camera->cursorVisible)
	{
		mousePosition.value = { x, y };
		shader->SetUniform(mousePosition);
		return;
	}

	glm::vec2 newPos = glm::vec2(x, y);
	if (camera->firstMouse)
	{
		camera->mouseOffset = newPos;
		camera->firstMouse = false;
	}

	camera->ProcessMouseMovement(glm::vec2(newPos.x - camera->mouseOffset.x, camera->mouseOffset.y - newPos.y) * camera->mouseSensitivity * camera->zoom.value); // reversed since y-coordinates go from bottom to top

	camera->mouseOffset = newPos;

	shader->SetUniform(camera->GetRotationMatrix());
}

// Map from screen space to fractal space
glm::vec2 Fractal::MapScreenMouseToFractal()
{
	if (shader->type == ShaderType::compute)
	{
		glm::vec4 renderArea = glm::vec4(0);
		for (size_t i = 0; i < fractalUniforms.size(); i++)
		{
			if (StringEqualNoCase(fractalUniforms[i].uniform->name, "renderArea"))
			{
				renderArea = ((Uniform<glm::vec4>*)fractalUniforms[i].uniform)->value;
				break;
			}
		}

		glm::vec2 midPoint = glm::vec2(abs(renderArea.x) - abs(renderArea.z), abs(renderArea.y) - abs(renderArea.w)) * glm::vec2(0.5);
		glm::vec4 area = (renderArea + glm::vec4(midPoint, midPoint)) * glm::vec4(camera->zoom.value) - glm::vec4(midPoint, midPoint);
		area += glm::vec4(glm::vec2(camera->position.value), glm::vec2(camera->position.value)) * glm::vec4(1, -1, 1, -1);


		glm::vec2 map = glm::vec2(screenSize.value / glm::vec2(area.z - area.x, area.w - area.y));
		if (renderArea != glm::vec4(0))
		{
			// Mapping from fractal space to screen space- simpy solve for coord x and y to go from screen to fractal
			/*int x = int((coord.x - area.x) * map.x - 0.5);
			int y = int(screenSize.y - (coord.y - area.y) * map.y);*/
			float x = (2 * mousePosition.value.x + 1) / (2 * map.x) + area.x;
			float y = area.y + mousePosition.value.y / map.y;
			return glm::vec2(x, y);
		}
	}

	// If we aren't using a compute shader or something fails, default to normal
	return (2.f * glm::vec2(mousePosition.value.x, screenSize.value.y - mousePosition.value.y) - (glm::vec2)screenSize.value) / (float)screenSize.value.y * camera->zoom.value + glm::vec2(camera->position.value);
}

void Fractal::MousePressCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (button == GLFW_MOUSE_BUTTON_2)
		{
			holdingMouse = true;

			if (!camera->viewMode3D.value)
			{
				// Map from screen space to fractal space
				clickPosition.SetValue(MapScreenMouseToFractal(), false);
				shader->SetUniform(clickPosition);
			}
		}
	}
	else if (action == GLFW_RELEASE)
	{
		holdingMouse = false;
	}
}


void Fractal::BuildMainLoop(Section targetSection, std::string& source, const std::string& defaultSource, std::string& target, std::string& specification, int* index, std::map<std::string, ShaderIndice*> indices)
{
	std::string distanceDeclaration;
	size_t distanceStart = source.find(targetSection.start);
	if (distanceStart == std::string::npos)
	{
		distanceStart = defaultSource.find(targetSection.start);
		if (distanceStart == std::string::npos)
		{
			size_t distanceTarget = target.find(targetSection.start);
			if (distanceTarget != std::string::npos)
			{
				Replace(target, targetSection.start, "");
			}
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

					int* relevantIndex = (indices.count(sectionName)) ? indices[sectionName]->value : index;


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

								// Key: parameter number. Value: array of starting and ending positions of various parameter occurences
								std::map<int, std::vector<std::pair<size_t, size_t>>> parameterLocations;


								const size_t parameterLength = std::string("parameter").size();
								size_t start = 0;
								while (true)
								{
									size_t parameterStart = newSection.find("parameter", start);

									if (parameterStart == std::string::npos)
									{
										break;
									}

									std::string numbers = "";
									size_t i;
									for (i = parameterStart + parameterLength; i < newSection.size(); i++)
									{
										// Check if current char is an integer
										if (newSection[i] > 47 && newSection[i] < 58)
										{
											numbers += newSection[i];
										}
										else
										{
											break;
										}
									}
									if (numbers == "") numbers = "0";

									int parameterNumber = std::stoi(numbers);
									if (parameterLocations[parameterNumber].size())
									{
										parameterLocations[parameterNumber].push_back(std::pair<size_t, size_t>(parameterStart, i));
									}
									else
									{
										parameterLocations[parameterNumber] = { std::pair<size_t, size_t>(parameterStart, i) };
									}

									start = i;
								}

								int offset = 0;
								for (auto const& x : parameterLocations)
								{
									for (size_t i = 0; i < x.second.size(); i++)
									{
										// Here lies the corpse of readability
										newSection.replace(x.second[i].first + offset, x.second[i].second - x.second[i].first, parameters[x.first]);
										offset += parameters[x.first].size() - (parameterLength + ((x.first == 0) ? 0 : std::to_string(x.first).size()));
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

void Fractal::BuildMainLoop(Section targetSection, std::string& source, const std::string& defaultSource, std::string& target, std::string& specification, std::map<std::string, ShaderIndice*> indices)
{
	int* index = new int(-1);
	BuildMainLoop(targetSection, source, defaultSource, target, specification, index, indices);
	delete index;
}

void Fractal::AddShaderParameters(std::string& spec)
{
	if (StringContainsNoCase(spec, "<shaderParameters>"))
	{
		std::string parameterStr = GetSection(Section("shaderParameters"), spec);
		std::vector<std::string> parameters = SplitNotInChar(parameterStr, ',', { { '{', '}' } });

		// Remove keys which were left from previous fractal
		std::vector<std::string> keysToRemove;
		for (auto& x : shaderIndices)
		{
			keysToRemove.push_back(x.first);
		}

		for (size_t i = 0; i < parameters.size(); i++)
		{
			std::vector<std::string> content = Split(parameters[i], ',');

			std::string name = GuiElement::GetElement(content, "name");
			CleanString(name, ' ');
			name = SubString(name, name.find_first_of(" \" ") + 1, name.find_last_of(" \" "));

			std::string valueStr = GuiElement::GetElement(content, "value");
			int value = 0;
			if (valueStr != "") value = std::stoi(valueStr);

			std::string key = GuiElement::GetElement(content, "key");
			if (key[0] == ' ') key = key.substr(1);
			key = SubString(key, key.find_first_of(" \" ") + 1, key.find_last_of(" \" "));

			std::vector<std::string> keyContent = Split(key, ' ');
			std::pair<int, int> keyUpDown{ -1, -1 };
			int modifier = 0;
			int offset = 0;
			if (keyContent[0].size() > 1)
			{
				std::string mod = keyContent[0];
				if (StringEqualNoCase(mod, "ctrl"))		  modifier = GLFW_MOD_CONTROL;
				else if (StringEqualNoCase(mod, "alt"))   modifier = GLFW_MOD_ALT;
				else if (StringEqualNoCase(mod, "shift")) modifier = GLFW_MOD_SHIFT;
				else if (StringEqualNoCase(mod, "super")) modifier = GLFW_MOD_SUPER;
				else BreakIfDebug();
				// If you broke here, we have got an unrecognized key modifier
				offset = 1;
			}

			keyUpDown.first  = toupper(keyContent[offset+0][0]);
			keyUpDown.second = toupper(keyContent[offset+1][0]);

			if (!shaderIndices.count(name))
			{
				shaderIndices[name] = new ShaderIndice(value, modifier, { keyUpDown.first, keyUpDown.second });
			}
			keysToRemove.erase(std::remove(keysToRemove.begin(), keysToRemove.end(), name), keysToRemove.end());
		}

		for (size_t i = 0; i < keysToRemove.size(); i++)
		{
			shaderIndices.erase(keysToRemove[i]);
		}
	}
}

void Fractal::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UNKNOWN) return; // Stay away from weird stuff
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, true); // Close program

	// Handle input actions in separate function
	if (keys[key] || ((mods & GLFW_MOD_CONTROL) != GLFW_MOD_CONTROL && (mods & GLFW_MOD_ALT) != GLFW_MOD_ALT))
	{
		if ((action == GLFW_PRESS)) keys[key] = true;
		else if ((action == GLFW_RELEASE)) keys[key] = false;
	}

	if (action == GLFW_PRESS)
	{
		bool update = false;

		if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
		{
			if (key == GLFW_KEY_Z) FindPathAndSaveImage();
			else if (key == GLFW_KEY_X) BreakIfDebug();
		}

		if (key == GLFW_KEY_X)
		{
			time.value.ToogleTimePause();
			shader->SetUniform(time);
		}

		if (key == GLFW_KEY_F)
		{
			camera->cursorVisible = !camera->cursorVisible;
			if (camera->cursorVisible)
			{
				double x;
				double y;
				glfwGetCursorPos(window, &x, &y);
				camera->lastNonGuiPos = { x,y };
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwSetCursorPos(window, camera->lastNonGuiPos.x, camera->lastNonGuiPos.y);
			}
		}

		for (auto const& x : shaderIndices)
		{
			if ((x.second->modifiers & mods) == x.second->modifiers)
			{
				if (key == x.second->button.first)  { (*x.second->value)--; update = true; }
				if (key == x.second->button.second) { (*x.second->value)++; update = true; }
			}
		}
		
		if (update)
		{
			UpdateFractalShader();
		}
	}
}

void Fractal::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	screenSize.value = glm::ivec2(width, height);
	shader->SetUniform(screenSize);

	if (shader->type == ShaderType::compute)
	{
		ComputeShader* compute = (ComputeShader*)shader;
		compute->UseRender();
		glUniform2f(compute->uniformRenderIds[screenSize.name], screenSize.value.x, screenSize.value.y);
		// What is rendered will most likely be trash, dispose of it
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, compute->mainBuffer.id);
		glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F, GL_RED, GL_FLOAT, nullptr);
		shader->Use();
	}

	glViewport(0, 0, width, height);
}

void Fractal::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Clamp yoffset. Extreme values break zooming due to precision
	yoffset = glm::clamp(yoffset, -1., 1.);
	camera->ProcessZoom(static_cast<float>(1 / (yoffset * camera->scrollSpeed + 1)));
	camera->zoom.SetGuiValue();
	shader->SetUniform(camera->zoom);
}

void Fractal::SetUniformLocations(Shader* shader, bool computeRender)
{
	unsigned int id = (computeRender) ? ((ComputeShader*)shader)->renderId : shader->id;
	if (computeRender) { ((ComputeShader*)shader)->UseRender(); }
	else { shader->Use(); }

	screenSize.id = glGetUniformLocation(id, screenSize.name.c_str());
	frame.id = glGetUniformLocation(id, frame.name.c_str());
	time.id = glGetUniformLocation(id, time.name.c_str());
	camera->zoom.id = glGetUniformLocation(id, camera->zoom.name.c_str());
	camera->GetRotationMatrix().id = glGetUniformLocation(shader->id, camera->GetRotationMatrix().name.c_str());
	camera->position.id = glGetUniformLocation(shader->id, camera->position.name.c_str());
	camera->viewMode3D.id = glGetUniformLocation(shader->id, camera->viewMode3D.name.c_str());
	mousePosition.id = glGetUniformLocation(id, mousePosition.name.c_str());
	clickPosition.id = glGetUniformLocation(id, clickPosition.name.c_str());
	
	GlErrorCheck();
}

void Fractal::SetUniforms(Shader* shader, bool computeRender)
{
	if (computeRender) { ((ComputeShader*)shader)->UseRender(); }
	else { shader->Use(); }

	shader->SetUniform(frame);
	shader->SetUniform(time);
	shader->SetUniform(screenSize);
	shader->SetUniform(camera->zoom);
	shader->SetUniform(camera->position);
	shader->SetUniform(camera->GetRotationMatrix());
	shader->SetUniform(camera->viewMode3D);
	shader->SetUniform(mousePosition);
	shader->SetUniform(clickPosition);
}

void Fractal::SetUniformNames()
{
	screenSize.name = "screenSize";
	frame.name = "frame";
	time.name = "time";
	camera->zoom.name = "zoom";
	camera->GetRotationMatrix().name = "rotation";
	camera->position.name = "position";
	camera->viewMode3D.name = "view3D";
	mousePosition.name = "mousePosition";
	clickPosition.name = "clickPosition";
}

// This is really nasty, be careful
void Fractal::SetVariable(std::string name, std::string value)
{
	if (StringEqualNoCase(name, "position"))
	{
		std::vector<std::string> components = Split(value, ',');
		glm::vec3 value;
		for (size_t i = 0; i < components.size(); i++)
		{
			value[i] = std::stof(components[i]);
		}
		camera->position.value = value;
	}
	else if (StringEqualNoCase(name, "yaw"))
	{
		camera->SetYaw(std::stof(value));
	}
	else if (StringEqualNoCase(name, "pitch"))
	{
		camera->SetPitch(std::stof(value));
	}
	else if (StringEqualNoCase(name, "mousePosition"))
	{
		std::vector<std::string> components = Split(value, ',');
		mousePosition.value = glm::vec2(std::stof(components[0]), std::stof(components[1]));
	}
}

std::vector<int> Fractal::GetPrimeFactors(int n)
{
	std::vector<int> factors = std::vector<int>();

	while (n % 2 == 0)
	{
		factors.push_back(2);
		n /= 2;
	}

	int root = int(ceil(sqrt(n)));

	for (int i = 3; i <= root; i += 2)
	{
		if (n % i == 0)
		{
			factors.push_back(i);
			n /= i;
		}
	}

	if (n > 2)
	{
		factors.push_back(n);
	}

	return factors;
}

void Fractal::ParseShader(std::string& source, std::string & final, const std::string* spec, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections)
{
	std::string defaultSource = FileManager::ReadFile(GetDefaultFractalSourcePath());

	std::map<ShaderSection, bool> sections = std::map<ShaderSection, bool>();

	std::string specSection = GetSpecificationByIndex(spec, specIndex, FileManager::ReadFile(GetPresetSpecPath()));
	if (specSection == "")
	{
		DebugPrint("Specification error");
		BreakIfDebug();
		return;
	}

	AddShaderParameters(specSection);

	std::string tip = GetSection(Section("tip"), specSection);
	if (tip != "") // Only print once
	{
		size_t start = tip.find("\"") + 1;
		size_t end = tip.find_last_of("\"");
		std::cout << tip.substr(start, end - start) << std::endl;
	}

	Section specUniforms = Section("uniforms");
	std::string finalUniforms;
	std::string uniformStr = GetSection(specUniforms, specSection);
	std::vector<std::string> uniforms = SplitNotInChar(uniformStr, ',', { { '<','>' } });
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		if (uniforms[i].size() > 5)
		{
			finalUniforms += uniforms[i].substr(1, uniforms[i].size() - 2) + "\n";
		}
	}

	size_t uniformsStart = final.find(specUniforms.start) + specUniforms.start.length();
	if (uniformsStart != std::string::npos)
	{
		final.insert(uniformsStart, finalUniforms);
	}


	Section help("helperFunctions");
	std::string functions = GetSection(help, source);


	Section include("include");
	std::string includes = GetSection(include, source);
	
	CleanString(includes, { ' ', '\t', '\n' });

	if (includes != "")
	{

		size_t includeStart = specSection.find("<include>");
		size_t includeEnd = specSection.find("</include>");
		if (includeStart != std::string::npos && includeEnd != std::string::npos)
		{
			std::string includeList = SubString(specSection, includeStart + std::string("<include>").size(), includeEnd);
			CleanString(includeList, { ' ', '\n', '\t' });
			if (includes[includes.size() - 1] != ',') includes += ",";

			includes += includeList;
		}

		std::vector<std::string> includeList = Fractal::Split(includes, ',');

		const std::string helperFunctions = FileManager::ReadFile(GetHelperFunctionPath());

		for (size_t i = 0; i < includeList.size(); i++)
		{
			std::string content = "";
			if ((content = GetSection(Section(includeList[i]), helperFunctions)) == "")
			{
				content = GetSection(Section(includeList[i]), source);
			}
			functions += content;
		}
	}

	Replace(final, help.start, functions);

	if (source.find(Section("mainLoopOff").start) == std::string::npos)
	{
		BuildMainLoop(Section((fractalType == FractalType::fractal2D) ? "mainLoop" : "distanceEstimator"), source, defaultSource, final, specSection, fractalIndex, shaderIndices);
	}
	else
	{
		Replace(final, Section("mainLoop").start, "");
	}

	std::string flags = GetSection(Section("flags"), specSection);

	std::vector<ShaderSection> shaderSections = GetShaderSections();
	for (size_t i = 0; i < shaderSections.size(); i++)
	{
		ShaderSection c = shaderSections[i];
		sections[c] = false;

		Section s = Section(c.name);

		if (flags.find("<" + GetSectionName(s.start) + "Default>") == std::string::npos)
		{
			sections[c] = ReplaceSection(s, Section(c.name), source, final);
			if (!sections[c])
			{
				sections[c] = ReplaceSection(s, Section(c.name), defaultSource, final);
			}
		}
	}




	Fractal::ParseShaderDefault(sections, source, final, specSection);

	for (size_t i = 0; i < extraSections.size(); i++)
	{
		ShaderSection c = extraSections[i];

		Section s = Section(c.name);

		std::string sectionString;
		if (c.multiple && source.find(s.start) != std::string::npos)
		{
			std::vector<std::string> versions;

			versions = (source.find(s.start) == std::string::npos) ? SplitNotInChar(GetSection(s, defaultSource), ',', '<', '>') : SplitNotInChar(GetSection(s, source), ',', '<', '>');
			std::string index = GetSection(s, specSection);

			sectionString = versions[std::stoi(index)];

			CleanString(sectionString, { '<', '>' });

			while (Replace(final, s.start, sectionString)) {}
		}
		else
		{
			sectionString = (source.find(s.end) != std::string::npos) ? GetSection(s, source) : GetSection(s, defaultSource);
			while (Replace(final, Section(c.name).start, sectionString)) {}
		}
	}
}

void Fractal::ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string & final, std::string specification)
{
	std::string defaultSource = FileManager::ReadFile(GetDefaultFractalSourcePath());


	if (specification.find(Section("variables").start) != std::string::npos)
	{
		std::vector<std::string> variables = SplitNotInChar(GetSection(Section("variables"), specification), ',', { {'<', '>'}, {'(', ')' }, { '[' , ']' } });
		for (size_t i = 0; i < variables.size(); i++)
		{
			std::string sectionName = GetSectionName(variables[i]);
			std::string value = GetSectionValue(variables[i]);

			size_t start;
			if ((start = final.find(sectionName)) != std::string::npos)
			{
				size_t end;
				end = final.find(';', start);

				std::string uniform = final.substr(start, end - start);


				if (value.find('[') != std::string::npos)
				{
					if (value[0] == '[') value = value.substr(1);
					if (value[value.size() - 1] == ']') value = value.substr(0, value.size() - 1);

					std::vector<std::string> versions = SplitNotInChar(value, ',', { { '[' , ']' } });
					if (versions.size() == 0)
					{
						DebugPrint("Splitting variables version on " + sectionName + " failed");
						BreakIfDebug();
					}

					for (size_t i = 0; i < versions.size(); i++)
					{
						CleanString(versions[i], { ' ' });
						if (versions[i][0] == '[') versions[i] = versions[i].substr(1);
						if (versions[i][versions[i].size() - 1] == ']') versions[i] = versions[i].substr(0, versions[i].size() - 1);
					}

					int index = 0;
					if (shaderIndices.size() && shaderIndices.count(sectionName))
					{
						index = *shaderIndices[sectionName]->value;
						if (index < 0)
						{
							index = versions.size() - 1;
						}
						if ((size_t)index >= versions.size())
						{
							index = 0;
						}

						*shaderIndices[sectionName]->value = index;
					}

					value = versions[index];
				}

				std::vector<std::string> uniformParts = SplitNotInChar(uniform, ' ', '(', ')');

				Replace(final, uniformParts[2], value, start);
			}
		}
	}

	const std::string alternateDefaultFunctions = FileManager::ReadFile(GetAlternateDefaultFunctionsPath());
	std::string alternateFunctionsStr = GetSection(Section("alternateFunctions"), specification);
	std::vector<std::string> alternateFunctions = Split(alternateFunctionsStr, ',');

	for (auto const& x : sections)
	{
		if (!x.second && x.first.optional)
		{
			Replace(final, Section(x.first.name).start, "");
		}
		else
		{
			Section s(x.first.name);

			std::string function = defaultSource;
			for (size_t i = 0; i < alternateFunctions.size(); i++)
			{
				if (GetSectionName(alternateFunctions[i]) == GetSectionName(s.start))
				{
					std::string functionName = GetSectionValue(alternateFunctions[i]);
					std::string newFunction = GetWholeSection(Section(functionName), alternateDefaultFunctions);
					function = newFunction;
					s = Section(functionName);
				}
			}

			ReplaceSection(s, Section(x.first.name), function, final);
		}
	}

	Section extraParameters("extraParameters");
	std::string parameters;
	bool replaced = false;
	if ((parameters = GetSection(extraParameters, source)) != "")
	{
		CleanString(parameters, { '[', ']', '\n' });
		std::vector<std::string> params = Split(parameters, ',');

		if (params.size() != 0)
		{
			std::string finalParams = "";
			for (size_t i = 0; i < params.size(); i++)
			{
				finalParams += "," + params[i];
			}

			replaced = Replace(final, extraParameters.start, finalParams);
		}
	}

	if (!replaced)
	{
		Replace(final, extraParameters.start, "");
	}

	std::string flags = GetSection(Section("flags"), specification);

	// Do this last, various reasons
	std::vector<ShaderSection> postShaderSections = GetPostShaderSections();
	for (size_t i = 0; i < postShaderSections.size(); i++)
	{
		Section s("");
		ShaderSection c = postShaderSections[i];
		if (c.optional && (source.find("<" + c.name + "Off>") != std::string::npos || flags.find("<" + c.name + "Off>") != std::string::npos))
		{
			Replace(final, Section(c.name).start, "");
			continue;
		}

		s = Section(c.name);

		std::string sectionString;
		if (c.multiple && source.find(s.start) != std::string::npos)
		{
			std::vector<std::string> versions;

			versions = SplitNotInChar(GetSection(s, (source.find(s.start) == std::string::npos) ? defaultSource : source), ',', '<', '>');
			std::string index = GetSection(s, specification);

			if (shaderIndices.size() != 0 && shaderIndices.count(c.name))
			{
				if (versions[versions.size() - 1][0] != '<') versions.pop_back();

				int* indexPtr = shaderIndices[c.name]->value;
				if (*indexPtr < 0) *indexPtr = versions.size() - 1;
				else if ((size_t)*indexPtr > versions.size() - 1) *indexPtr = 0;
				index = std::to_string(*indexPtr);
			}
			else if (index == "") index = "0";


			size_t indexInt = std::stoi(index);
			if (indexInt > versions.size() - 1)
			{
				DebugPrint("Index was " + std::to_string(indexInt) + ", which is out of bounds at " + c.name);
				BreakIfDebug();
				continue;
			}
			sectionString = versions[indexInt];

			if (sectionString[0] == '<') sectionString.erase(0, 1);
			if (sectionString[sectionString.length() - 1] == '>') sectionString.erase(sectionString.length() - 1);
		}
		else
		{
			sectionString = (source.find(s.start) == std::string::npos) ? GetSection(s, defaultSource) : GetSection(s, source);
		}

		while (Replace(final, s.start, sectionString)) {}
	}
}

Shader* Fractal::CreateShader(std::string source, const std::string* specification, int* fractalIndex, int* specIndex, std::vector<ShaderSection> shaderSections)
{
	const static std::string vertexSource = FileManager::ReadFile(Fractal::pathRectangleVertexshader);

	std::string renderBase;
	std::string type = GetSection(Section("type"), source);
	if (type != "")
	{
		if (type == "fragment")
		{
			renderBase = FileManager::ReadFile(GetBasePath());
		}
		else if (type == "compute")
		{
			const static int maxDimensions = 3;


			std::string dimensionNumber = GetSection(Section("localSizeDimensions"), source);
			if (dimensionNumber == "") dimensionNumber = "1";

			int dimensions;
			dimensions = std::stoi(dimensionNumber);
			if (dimensions > maxDimensions) dimensions = maxDimensions;

			int workGroupMaxProduct;
			glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupMaxProduct);

			int maxProductRoot = (int)std::floor(pow((double)workGroupMaxProduct, 1. / dimensions));

			std::vector<int> factors[3] = { GetPrimeFactors(int(screenSize.value.x)), GetPrimeFactors(int(screenSize.value.y)), {1} };

			int workGroups[maxDimensions] = { 1, 1, 1 };
			for (int i = 0; i < dimensions; i++)
			{
				int maxGroupSize;
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, i, &maxGroupSize);
				for (size_t j = 0; j < factors[i].size(); j++)
				{
					workGroups[i] *= factors[i][j];
					if (workGroups[i] > maxGroupSize || workGroups[i] > maxProductRoot)
					{
						workGroups[i] /= factors[i][j];
						break;
					}
				}
			}

			std::string computeBase = FileManager::ReadFile(GetComputeBasePath());

			if (!Replace(computeBase, "<sizeX>", std::to_string(workGroups[0]))) DebugPrint("Could not replace compute shader <sizeX> with " + std::to_string(workGroups[0]));
			if (!Replace(computeBase, "<sizeY>", std::to_string(workGroups[1]))) DebugPrint("Could not replace compute shader <sizeY> with " + std::to_string(workGroups[1]));
			if (!Replace(computeBase, "<sizeZ>", std::to_string(workGroups[2]))) DebugPrint("Could not replace compute shader <sizeZ> with " + std::to_string(workGroups[2]));

			if (source.find("std430" != 0))
			{
				Replace(computeBase, "#version 330", "#version 430");
			}

			int renderingFrequency;
			std::string renderingFrequencyStr = GetSection(Section("renderFrequency"), source);

			renderingFrequency = (renderingFrequencyStr == "") ? ComputeShader::DefaultRenderingFrequency : std::stoi(renderingFrequencyStr);


			std::string renderSourceName = GetSection(Section("render"), source);
			std::string renderSource;

			if (renderSourceName == "")
			{
				std::cout << "Could not find render source";
				BreakIfDebug();
			}
			renderSource = FileManager::ReadFile(GetFractalPath() + renderSourceName);

			std::string uniforms = GetSection(Section("uniforms"), renderSource);
			if (uniforms != "")
			{
				size_t sourceUniformStart = source.find(Section("uniforms").start);
				if (sourceUniformStart != std::string::npos)
				{
					source.insert(sourceUniformStart + Section("uniforms").start.size(), uniforms);
				}
			}

			ParseShader(source, computeBase, specification, specIndex, fractalIndex, shaderSections);


			std::string renderBase = FileManager::ReadFile(GetBasePath());

			ParseShader(renderSource, renderBase, specification, specIndex, fractalIndex, shaderSections);

			if (renderBase.find("std430" != 0))
			{
				Replace(renderBase, "#version 330", "#version 430");
			}


			fractalSourceCode = computeBase;


#if PrintSource
			std::cout << renderBase;
			std::cout << computeBase;
#endif

			return new ComputeShader(computeBase, vertexSource, renderBase, false, { workGroups[0], workGroups[1], workGroups[2] }, renderingFrequency);
		}
	}
	else // Assume fragment shader
	{
		renderBase = FileManager::ReadFile(GetBasePath());
	}

	if (source.find("std430" != 0))
	{
		Replace(renderBase, "#version 330", "#version 430");
	}

	ParseShader(source, renderBase, specification, specIndex, fractalIndex, shaderSections);

#if PrintSource
	std::cout << renderBase;
#endif


	fractalSourceCode = renderBase;

	return new Shader(vertexSource, renderBase, false);
}

std::string Fractal::GetDefaultFractalSourcePath()
{
	return (fractalType == FractalType::fractal2D) ? "shaders/2D/Base/2DFractalDefault.fs" : "shaders/3D/Base/3DFractalDefault.fs";
}

std::string Fractal::GetAlternateDefaultFunctionsPath()
{
	return (fractalType == FractalType::fractal2D) ? "shaders/2D/Base/AlternateDefault.fs" : "shaders/3D/Base/AlternateDefault.fs";
}

std::string Fractal::GetPresetSpecPath()
{
	return (fractalType == FractalType::fractal2D) ? "shaders/2D/Base/PresetSpecs.fs" : "shaders/3D/Base/PresetSpecs.fs";
}

std::string Fractal::GetHelperFunctionPath()
{
	return (fractalType == FractalType::fractal2D) ? "shaders/2D/Base/HelperFunctions.fs" : "shaders/3D/Base/HelperFunctions.fs";
}

std::string Fractal::GetBasePath()
{
	return (fractalType == FractalType::fractal2D) ? "shaders/2D/Base/2DFractalBase.fs" : "shaders/3D/Base/3DFractalbase.fs";
}

std::string Fractal::GetComputeBasePath()
{
	return (fractalType == FractalType::fractal2D) ? "shaders/2D/Base/2DFractalBaseCompute.fs" : "shaders/3D/Base/3DFractalBaseCompute.fs";
}

std::string Fractal::GetFractalPath()
{
	return (fractalType == FractalType::fractal2D) ? "shaders/2D/Fractals/" : "shaders/3D/Fractals/";
}

std::string Fractal::GetFractalFolderPath()
{
	return (fractalType == FractalType::fractal2D) ? "/shaders/2D/Fractals/" : "/shaders/3D/Fractals/";
}

std::string Fractal::GetSpecPath(std::string fileName)
{
	return GetFractalPath() + fileName + "Specs.fs";
}

std::string Fractal::GetFractalPath(std::string fileName)
{
	return GetFractalPath() + fileName + ".fs";
}

std::vector<ShaderSection> Fractal::GetPostShaderSections()
{
	return (fractalType == FractalType::fractal2D) 
		?	std::vector<ShaderSection>{ ShaderSection("coloring", false, true) }
		:	std::vector<ShaderSection>{ ShaderSection("coloring", false, true), ShaderSection("edgeGlow", false, true), ShaderSection("sky", true), ShaderSection("sun", true),
										ShaderSection("distanceBody"), ShaderSection("trap") };
}

std::vector<ShaderSection> Fractal::GetShaderSections()
{
	return (fractalType == FractalType::fractal2D)
		? std::vector<ShaderSection>{ ShaderSection("constants", true), ShaderSection("uniforms", true), ShaderSection("buffers", true), ShaderSection("main", false), }
		: std::vector<ShaderSection>{ ShaderSection("constants", true), ShaderSection("uniforms", true), ShaderSection("buffers"), ShaderSection("sceneDistance"),
									  ShaderSection("trace"), ShaderSection("render"), ShaderSection("main", false), ShaderSection("lightingFunctions") };
}

Shader* Fractal::GenerateShader(int* specIndex, int* fractalIndex, std::string name)
{
	GlErrorCheck();

	std::vector<ShaderSection> sections{};

	std::string source = FileManager::ReadFile(GetFractalPath(name));

	Section extraSects = Section("extraSections");
	size_t extraSectionIndex = source.find(extraSects.start);
	if (extraSectionIndex != std::string::npos)
	{
		size_t extraSectionEnd = source.find(extraSects.end);
		std::vector<std::string> sectionContents = SplitNotInChar(source.substr(extraSectionIndex + extraSects.start.length(), extraSectionEnd - (extraSectionIndex + extraSects.start.length())), ',', '[', ']');
		for (size_t i = 0; i < sectionContents.size(); i++)
		{
			CleanString(sectionContents[i], { '\n', '\t', ' ', '[', ']', '\"' });
			std::vector<std::string> value = Split(sectionContents[i], ',');
			if (value.size() == 1)	sections.push_back(ShaderSection(value[0]));
			else if (value.size() == 2) sections.push_back(ShaderSection(value[0], StringToBool(value[1])));
			else if (value.size() == 4) sections.push_back(ShaderSection(value[0], StringToBool(value[1]), StringToBool(value[3])));
		}
	}

	const std::string specification = FileManager::ReadFile(GetSpecPath(name));


	return CreateShader(source, &specification, fractalIndex, specIndex, sections);
}

Shader* Fractal::GenerateShader(int specIndex, int fractalIndex, std::string fractalName)
{
	int* specIndexPtr = new int(specIndex);
	int* fractalIndexPtr = new int(fractalIndex);
	Shader* shader = GenerateShader(specIndexPtr, fractalIndexPtr, fractalName);
	delete specIndexPtr;
	delete fractalIndexPtr;
	return shader;
}

Shader* Fractal::GenerateShader(std::string name)
{
	int* specIndex = new int(0);
	int* fractalIndex = new int(0);
	Shader* shader = GenerateShader(specIndex, fractalIndex, fractalName);
	delete specIndex;
	delete fractalIndex;
	return shader;
}

Shader* Fractal::GenerateShader()
{
	return GenerateShader(&specIndex, &fractalIndex, fractalName);
}

void Fractal::HandleKeyInput()
{
	for (auto const& key : keys)
	{
		if (key.second)
		{
			switch (key.first)
			{
				// Zooming using exponential decay
			case GLFW_KEY_Q:
				camera->ProcessZoom(static_cast<float>(exp(time.value.GetDeltaTime() * -parameterChangeRate)));
				camera->zoom.SetGuiValue();
				shader->SetUniform(camera->zoom);
				break;
			case GLFW_KEY_E:
				camera->ProcessZoom(static_cast<float>(1/exp(time.value.GetDeltaTime() * -parameterChangeRate)));
				camera->zoom.SetGuiValue();
				shader->SetUniform(camera->zoom);
				break;

				// WASD movement
			case GLFW_KEY_W:
				camera->ProcessMovement(CameraMovement::forward, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * camera->zoom.value);
				shader->SetUniform(camera->position);
				break;
			case GLFW_KEY_S:
				camera->ProcessMovement(CameraMovement::back, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * camera->zoom.value);
				shader->SetUniform(camera->position);
				break;
			case GLFW_KEY_A:
				camera->ProcessMovement(CameraMovement::left, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * camera->zoom.value);
				shader->SetUniform(camera->position);
				break;
			case GLFW_KEY_D:
				camera->ProcessMovement(CameraMovement::right, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * camera->zoom.value);
				shader->SetUniform(camera->position);
				break;

				// Up and down
			case GLFW_KEY_SPACE:
				camera->ProcessMovement(CameraMovement::up, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * camera->zoom.value);
				shader->SetUniform(camera->position);
				break;
			case GLFW_KEY_LEFT_SHIFT:
				camera->ProcessMovement(CameraMovement::down, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * camera->zoom.value);
				shader->SetUniform(camera->position);
				break;

			default:
				break;
			}
		}
	}
}

void Fractal::Init()
{
	frame.value = 0;

	SetFractalNameFromIndex(&fractalNameIndex, GetFractalPath());
	SetVariablesFromSpec(&specIndex, GetSpecPath(fractalName), GetPresetSpecPath());

	SetUniformNames();

	SetUniformLocations(shader);
	SetUniforms(shader);
	shader->Use();
	GlErrorCheck();

	PopulateGUI();
	PopulateGuiFromShader();
	gui->performLayout();


	if (shader->type == ShaderType::compute)
	{
		ComputeShader* compute = reinterpret_cast<ComputeShader*>(shader);

		compute->UseRender();
		unsigned int id = compute->renderId;
		compute->uniformRenderIds[time.name] = glGetUniformLocation(id, time.name.c_str());
		compute->uniformRenderIds[frame.name] = glGetUniformLocation(id, frame.name.c_str());
		compute->uniformRenderIds[deltaTime.name] = glGetUniformLocation(id, deltaTime.name.c_str());
		compute->uniformRenderIds[screenSize.name] = glGetUniformLocation(id, screenSize.name.c_str());
		glUniform2f(compute->uniformRenderIds[screenSize.name], Fractal::screenSize.value.x, Fractal::screenSize.value.y);
		shader->Use();
	}

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
