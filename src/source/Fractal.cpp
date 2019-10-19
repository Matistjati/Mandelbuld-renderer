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

#define DoNothing [](){}
#define EmptyThread std::thread(DoNothing)

Uniform<glm::ivec2> Fractal::screenSize;
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
			std::vector<std::string> innerSections = SplitNotInChar(sections[i], ',', '[', ']');
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

Fractal::Fractal(std::pair<Shader*, Shader*> shaders, Uniform<glm::ivec2> screenSize, Time t, std::map<std::string, int*> shaderIndices, float zoom, FractalType f, int fractalIndex,
	int specIndex, int fractalNameIndex, std::string fractalName)
	: explorationShader(shaders.first), renderShader(shaders.second), zoom(zoom), fractalType(f), time(t, "time", glGetUniformLocation(shaders.first->id, "time")), deltaTime(0, "deltaTime", glGetUniformLocation(shaders.first->id, "deltaTime")),
	fractalIndex(fractalIndex), specIndex(specIndex), fractalName(fractalName), fractalNameIndex(fractalNameIndex), shaderIndices(shaderIndices), holdingMouse(false), fractalUniforms(), fractalSourceCode((fractalSourceCode == "") ? "" : fractalSourceCode)
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

	glViewport(0, 0, fractal->screenSize.value.x, fractal->screenSize.value.y);

	if (fractal->fractalType == FractalType::fractal3D)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	fractal->explorationShader->use();
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

		fractal->explorationShader->use();
		if (fractal->explorationShader->type == ShaderType::fragment)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractal->explorationShader->buffers[Fractal::rectangleVertexBufferIndexName].id);
			glBindVertexArray(fractal->explorationShader->buffers[Fractal::rectangleVertexBufferName].id);
			// rendering, we use ray marching inside the fragment shader
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		else if (fractal->explorationShader->type == ShaderType::compute)
		{
			ComputeShader* compute = reinterpret_cast<ComputeShader*>(fractal->explorationShader);
			compute->Invoke(fractal->screenSize.value);
			if (fractal->frame.value % compute->renderingFrequency == 0)
			{
				// Draw to both front and back buffers to avoid stuttering
				reinterpret_cast<Fractal2D*>(fractal)->RenderComputeShader();
				glfwSwapBuffers(window);
				reinterpret_cast<Fractal2D*>(fractal)->RenderComputeShader();
				glfwSwapBuffers(window);
			}
		}
		fractal->gui->drawContents();
		fractal->gui->drawWidgets();
		fractal->explorationShader->use();

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
	const size_t pixelCount = fractal->screenSize.value.x * fractal->screenSize.value.y;


	//fractal->zoom.value = 0.0150609445;
	//(reinterpret_cast<Fractal2D*>(fractal))->position.value.x = -0.73962032;
	//(reinterpret_cast<Fractal2D*>(fractal))->position.value.y = 0.210820600;
	fractal->zoom.value = 2.16f;
	(reinterpret_cast<Fractal2D*>(fractal))->position.value.x = -0.18303f;
	(reinterpret_cast<Fractal2D*>(fractal))->position.value.y = 0.f;
	*fractal->shaderIndices["loopExtraOperations"] = 2;
	*fractal->shaderIndices["loopReturn"] = 1;
	fractal->UpdateFractalShader();

	fractal->renderShader->use();
	fractal->SetUniformLocations(fractal->renderShader);
	fractal->SetUniforms(fractal->renderShader);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractal->explorationShader->buffers[Fractal::rectangleVertexBufferIndexName].id);
	glBindVertexArray(fractal->explorationShader->buffers[Fractal::rectangleVertexBufferName].id);


	std::vector<glm::ivec4> images = std::vector<glm::ivec4>(pixelCount);
	std::vector<Pixel> currentImage = std::vector<Pixel>(pixelCount);

	GlErrorCheck();
	for (double time = 0; time < pi2; time += dt)
	{
		fractal->time.value.SetTotalTime(time);
		fractal->explorationShader->SetUniform(fractal->time);


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glReadPixels(0, 0, fractal->screenSize.value.x, fractal->screenSize.value.y, GL_RGBA, GL_UNSIGNED_BYTE, &currentImage[0]);


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

	Image image(fractal->screenSize.value.x, fractal->screenSize.value.y, images);
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

	const size_t pixelCount = fractal->screenSize.value.x * fractal->screenSize.value.y;

	glfwSetCursorPosCallback(window, MouseCallbackDelegate);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallbackDelegate);
	glfwSetKeyCallback(window, KeyCallbackDelegate);
	glfwSetScrollCallback(window, ScrollCallBackDelegate);

	glViewport(0, 0, fractal->screenSize.value.x, fractal->screenSize.value.y);

	if (fractal->fractalType == FractalType::fractal3D)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	fractal->explorationShader->use();
	GlErrorCheck();

	int count = 0;
	// Find the current lowest count
	while (FileManager::FileExists((baseName + std::to_string(count) + ".png"))) count++;

	std::vector<Pixel> data = std::vector<Pixel>(pixelCount);

	// Ping pong buffering with pbos to render quicker
	unsigned int pboIds[2];
	glGenBuffers(2, pboIds);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, screenSize.value.x*screenSize.value.y*4, 0, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[1]);
	glBufferData(GL_PIXEL_PACK_BUFFER, screenSize.value.x * screenSize.value.y * 4, 0, GL_STREAM_READ);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	// Set dt
	fractal->deltaTime.value = (float)dt;
	fractal->explorationShader->SetUniform(fractal->deltaTime);

	// render loop
	double imageRenderTime = glfwGetTime();
	std::thread imageSaveThread(DoNothing);
	Image image(screenSize.value.x, screenSize.value.y, &data);

	for (size_t i = 0; i < imageCount; i++)
	{
		if (fractal->explorationShader->type == ShaderType::compute)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ((ComputeShader*)fractal->explorationShader)->mainBuffer.id);
			glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F, GL_RED, GL_FLOAT, nullptr);
		}

		fractal->explorationShader->use();
		fractal->frame.value = 0;
		fractal->explorationShader->SetUniform(fractal->frame);
		fractal->explorationShader->SetUniform(fractal->time.id, float(i*dt));


#if _DEBUG
		// Set the window title to our fps
		std::string title = std::to_string(glfwGetTime() - imageRenderTime);
		glfwSetWindowTitle(window, title.c_str());
#endif
		imageRenderTime = glfwGetTime();
		for (size_t j = 0; j < framesPerImage; j++)
		{
			if (fractal->explorationShader->type == ShaderType::fragment)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractal->explorationShader->buffers[Fractal::rectangleVertexBufferIndexName].id);
				glBindVertexArray(fractal->explorationShader->buffers[Fractal::rectangleVertexBufferName].id);
				// rendering, we use ray marching inside the fragment shader
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			else if (fractal->explorationShader->type == ShaderType::compute)
			{
				reinterpret_cast<ComputeShader*>(fractal->explorationShader)->Invoke(fractal->screenSize.value);
			}
			fractal->frame.value++;
			fractal->explorationShader->SetUniform(fractal->frame);
		}

		if (fractal->explorationShader->type == ShaderType::fragment)
		{
			fractal->FindPathAndSaveImage();
		}
		else if (fractal->explorationShader->type == ShaderType::compute)
		{
			// Ping pong buffering
			static int index = 0;
			int nextIndex = 0;

			// increment current index first then get the next index
			// "index" is used to read pixels from a framebuffer to a PBO
			// "nextIndex" is used to process pixels in the other PBO
			index = (index + 1) % 2;
			nextIndex = (index + 1) % 2;

			fractal->renderShader->use();
			fractal->renderShader->SetUniform(fractal->frame);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			fractal->explorationShader->use();

			imageSaveThread.join();

			std::string path;
			while (FileManager::FileExists(path = (baseName + std::to_string(count) + ".png"))) count++;

			glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[index]);
			glReadPixels(0, 0, screenSize.value.x, screenSize.value.y, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ((ComputeShader*)fractal->explorationShader)->mainBuffer.id);

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
				data.assign(src, src + screenSize.value.x * screenSize.value.y);
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
					data.assign(src, src + screenSize.value.x * screenSize.value.y);
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

void Fractal::UpdateFractalShader()
{
	delete this->renderShader;
	delete this->explorationShader;

	std::pair<Shader*, Shader*> shaders = this->GenerateShader();
	this->explorationShader = shaders.first;
	this->renderShader = shaders.second;

	// Resetting the content in the GUI
	while (gui->childCount())
	{
		gui->removeChild(gui->childAt(0));
	}
	gui->ClearFocusPath();
	gui->nanoGuiWindow = gui->form->addWindow(Eigen::Vector2i(10, 10), gui->guiWindowName);

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

		hintStart += hintS.length();

		std::string paramsStr = source.substr(hintStart, hintEnd - hintStart);
		std::vector<std::string> params = SplitNotInChar(paramsStr, ',', { {'(',')'} });

		size_t uniformEnd = source.find(";", hintEnd);
		if (uniformEnd == std::string::npos) break;

		hintEnd += hintE.length();

		std::string uniform = source.substr(hintEnd, uniformEnd - hintEnd);

		// Removing start of comment
		if (uniform[0] == '*') uniform = uniform.substr(1);
		if (uniform[0] == '/') uniform = uniform.substr(1);

		CleanString(uniform, { '\n' });

		std::vector<std::string> uniformParts = SplitNotInChar(uniform, ' ', { {'(',')'} });

		std::string type = GuiElement::GetElement(params, "Type");
		CleanString(type, { ' ' });

		std::string name = GuiElement::GetElement(params, "Name");
		if (name[0] == ' ') name = name.substr(1);

		GuiElement element = GuiElement(GuiElement::GetElementFromString(type), uniformParts[1], uniformParts[2], name, this, uniformParts[uniformParts.size() - 1], params);

		fractalUniforms.push_back(element);

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

}

void Fractal::SetShaderGui(bool render)
{
	bool preserveOld = Fractal::renderMode;

	Fractal::renderMode = render;

	for (auto& uni : fractalUniforms)
	{
		uni.SetGuiValue();
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

glm::ivec2 Fractal::GetMonitorSize()
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

	return result;
}

void Fractal::PopulateGUI()
{
	gui->form->setFixedSize({ 80,30 });

	gui->form->addGroup("General variables");


	// Zoom
	auto zoomField = gui->form->addVariable("Zoom", zoom.value);
	zoomField->setCallback([this](float value)
		{
			this->zoom.GetValue() = value;
			this->explorationShader->SetUniform(this->zoom);
		});

	zoom.guiElements = { zoomField };
	zoom.SetGuiValue = [this]() { ((nanogui::detail::FormWidget<float, std::true_type>*)this->zoom.guiElements[0])->setValue(this->zoom.GetValue()); };
	
	auto renderCheckbox = gui->form->AddCheckbox("Render Mode", renderMode);
	renderCheckbox->setCallback([this](bool value)
		{
			this->renderMode = value;
			this->SetShaderGui(value);
		});

	// Time
	double dummy;
	auto timeField = gui->form->addVariable("Time", dummy);
	timeField->setCallback([this](double value)
		{
			value = std::max(value, 0.);
			this->time.value.SetTotalTime(value);
			this->explorationShader->SetUniform(this->time);
		});

	time.guiElements = { timeField };
	time.SetGuiValue = [this]() { ((nanogui::detail::FormWidget<double, std::true_type>*)this->time.guiElements[0])->setValue(this->time.value.GetTotalTime()); };



	// ParameterChangeRate
	nanogui::Slider* paramSlider = gui->form->AddSlider("ParameterChangeRate", parameterChangeRate);

	paramSlider->setValue(parameterChangeRate);
	paramSlider->setRange({ 0.000001f,10.f });
}

void Fractal::Update()
{
	frame.value++;
	explorationShader->SetUniform(frame);

	time.value.PollTime();
	explorationShader->SetUniform(time);
	time.SetGuiValue();
	deltaTime.value = (float)time.value.GetDeltaTime();
	explorationShader->SetUniform(deltaTime);
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


					if (*relevantIndex < 0)* relevantIndex = sequences.size() - 1;
					else if (*relevantIndex > (int)sequences.size() - 1)* relevantIndex = 0;

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
								Replace(newSection, "parameter", parameterValue);
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
										size_t start = newSection.find("parameter");
										if (start == std::string::npos) break;

										char parameterPostfix = newSection[start + std::string("parameter").length()];
										if (parameterPostfix < 48 || parameterPostfix > 57) // The postfix isn't a number
										{
											Replace(newSection, "parameter", parameters[0]);
										}
										else
										{
											break;
										}
									}
								}

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

void Fractal::HandleKeyInput()
{
	
}
