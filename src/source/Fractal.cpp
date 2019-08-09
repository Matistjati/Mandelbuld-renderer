#include "headers/Fractal.h"
#include "headers/Fractal2D.h"
#include "headers/Fractal3D.h"
#include "headers/Debug.h"
#include "headers/Image.h"
#include <headers/FileManager.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

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
	if (startIndex == std::string::npos || endIndex == std::string::npos)
	{
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

	CleanString(str, { '\n','\t', ' ' });

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
	if (index < 0) index = 0;
	else if (*index > n - 1) (*index) = n - 1;

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
					target.insert(sectionStart + 1, innerSections[i]+ ",");
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

void MouseCallbackDelegate(GLFWwindow* window, double xpos, double ypos)
{
	switch ((reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)))->fractalType)
	{
	default:
		DebugPrint("Case default reached in function MouseCallback");
		break;
	case FractalType::fractal2D:
		(reinterpret_cast<Fractal2D*>(glfwGetWindowUserPointer(window)))->MouseCallback(window, xpos, ypos);
		break;
	case FractalType::fractal3D:
		(reinterpret_cast<Fractal3D*>(glfwGetWindowUserPointer(window)))->MouseCallback(window, xpos, ypos);
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


void KeyCallbackDelegate(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS && (mods & GLFW_MOD_ALT) == GLFW_MOD_ALT)
	{
		Fractal* fractal = reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window));
		bool update = true;
		switch (key)
		{
		default:
			update = false;
			break;
		case GLFW_KEY_Q:
			fractal->specIndex++;
			fractal->fractalIndex = 0;
			break;
		case GLFW_KEY_A:
			fractal->specIndex--;
			fractal->specIndex = std::max(fractal->specIndex, 0);
			fractal->fractalIndex = 0;
			break;

		case GLFW_KEY_W:
			fractal->fractalIndex++;
			break;
		case GLFW_KEY_S:
			fractal->fractalIndex--;
			fractal->fractalIndex = std::max(fractal->fractalIndex, 0);
			break;

		case GLFW_KEY_E:
			fractal->fractalNameIndex++;
			fractal->fractalIndex = 0;
			fractal->specIndex = 0;
			fractal->SetFractalNameFromIndex(&fractal->fractalNameIndex, fractal->GetFractalFolderPath());
			break;
		case GLFW_KEY_D:
			fractal->fractalNameIndex--;
			fractal->fractalIndex = 0;
			fractal->specIndex = 0;
			fractal->SetFractalNameFromIndex(&fractal->fractalNameIndex, fractal->GetFractalFolderPath());
			break;

		case GLFW_KEY_R:
			fractal->fractalNameIndex = 0;
			fractal->fractalIndex = 0;
			fractal->specIndex = 0;
			if (fractal->fractalType == FractalType::fractal2D)
			{
				glfwSetWindowUserPointer(window, new Fractal3D(fractal->specIndex, fractal->fractalIndex, fractal->fractalNameIndex, fractal->screenSize.value));
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else
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

void Fractal::RenderLoop(GLFWwindow* window, Fractal* fractal)
{
	glfwSetCursorPosCallback(window, MouseCallbackDelegate);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallbackDelegate);
	glfwSetKeyCallback(window, KeyCallbackDelegate);
	glfwSetScrollCallback(window, ScrollCallBackDelegate);

	glViewport(0, 0, fractal->screenSize.value.x, fractal->screenSize.value.y);

	if (fractal->fractalType == fractal3D)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	fractal->explorationShader->use();
	GlErrorCheck();

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		fractal->time.value.PollTime();
		fractal->explorationShader->SetUniform(fractal->time);

		fractal->Update();

#if _DEBUG
		// Set the window title to our fps
		std::string title = std::to_string(1 / fractal->time.value.GetDeltaTime());
		glfwSetWindowTitle(window, title.c_str());
#endif

		fractal->explorationShader->use();
		if (fractal->explorationShader->type == fragment)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractal->explorationShader->buffers[Fractal::rectangleVertexBufferIndexName].id);
			glBindVertexArray(fractal->explorationShader->buffers[Fractal::rectangleVertexBufferName].id);
			// rendering, we use ray marching inside the fragment shader
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		else if (fractal->explorationShader->type == compute)
		{
			reinterpret_cast<ComputeShader*>(fractal->explorationShader)->Invoke(fractal->screenSize.value);
		}


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
	for (double time = 0; time < pi2; time+=dt)
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

void Fractal::SetFractalNameFromIndex(int* index, std::string fractalPath)
{
	std::vector<std::string> fractals = FileManager::GetDirectoryFileNames(fractalPath);
	std::vector<std::string> fractalNames = GetFractalNames(fractals);
	*index = std::max(*index, 0);
	if (*index > (int)fractalNames.size() - 1)* index = (int)fractalNames.size() - 1;
	fractalName = fractalNames[*index];
}

void Fractal::UpdateFractalShader()
{
	delete this->renderShader;
	delete this->explorationShader;

	std::pair<Shader*, Shader*> shaders = this->GenerateShader();
	this->explorationShader = shaders.first;
	this->renderShader = shaders.second;
	if (fractalType == fractal3D)
	{
		(reinterpret_cast<Fractal3D*> (this))->Init();
	}
	else if (fractalType == fractal2D)
	{
		(reinterpret_cast<Fractal2D*> (this))->Init();
	}
	else
	{
		DebugPrint("Case default reached in UpdateFractalShader");
	}
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


					if (*relevantIndex < 0) *relevantIndex = 0;
					else if (*relevantIndex > (int)sequences.size() - 1) *relevantIndex = sequences.size() - 1;

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

					final += newSection;
				}

				Replace(target, Section(sectionName).start, final);
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
