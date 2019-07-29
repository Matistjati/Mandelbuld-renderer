#include "headers/Fractal2D.h"
#include "headers/FileManager.h"
#include "headers/Debug.h"
#include "headers/Image.h"
#include <algorithm>

const std::string& Fractal2D::default2DSource = FileManager::ReadFile(default2DFractal);

Fractal2D::Fractal2D(int specIndex, int fractalIndex, int fractalNameIndex, glm::ivec2 screenSize)
	: Fractal(GenerateShader(specIndex, fractalIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()))[fractalNameIndex]),
	screenSize, Time(), {}), power(2), position({ 0,0 })
{
	Init();
}

Fractal2D::Fractal2D(int specIndex, int fractalIndex, int fractalNameIndex)
	: Fractal(GenerateShader(specIndex, fractalIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()))[fractalNameIndex]),
	GetMonitorSize(), Time(), {}), power(2), position({ 0,0 })
{
	Init();
}

void Fractal2D::Update()
{
	frame.value++;
	explorationShader->SetUniform(frame);
}

void Fractal2D::MouseCallback(GLFWwindow* window, double x, double y)
{
	mousePosition.value = { x, y };
	explorationShader->SetUniform(mousePosition);
}

void Fractal2D::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UNKNOWN) return; // Stay away from weird stuff
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, true); // Close program

	// Handle input actions in separate function
	if ((mods & GLFW_MOD_CONTROL) != GLFW_MOD_CONTROL && (mods & GLFW_MOD_ALT) != GLFW_MOD_ALT)
	{
		if ((action == GLFW_PRESS)) keys[key] = true;
		else if ((action == GLFW_RELEASE)) keys[key] = false;
	}


	// Ctrl key handling
	if (action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
	{
		bool update = false;
		switch (key)
		{
		default:
			break;
		case GLFW_KEY_Z:
			FindPathAndSaveImage();
			break;


		case GLFW_KEY_X:
			BreakIfDebug();
			break;
		}
		if (update)
		{
			UpdateFractalShader();
		}
	}
	else if (action == GLFW_PRESS)
	{
		switch (key)
		{
			case GLFW_KEY_X:
				time.value.ToogleTimePause();
				explorationShader->SetUniform(time);
				break;

		}
	}
}

void Fractal2D::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	screenSize.value = glm::ivec2(width, height);
	explorationShader->SetUniform(screenSize);

	glViewport(0, 0, width, height);
}

void Fractal2D::SetUniforms(Shader* shader)
{
	shader->use();
	shader->SetUniform(position);
	shader->SetUniform(screenSize);
	shader->SetUniform(power);
	shader->SetUniform(zoom);
	shader->SetUniform(mousePosition);
	shader->SetUniform(time);
	shader->SetUniform(frame);
	GlErrorCheck();
}

void Fractal2D::SetUniformLocations(Shader* shader)
{
	shader->use();
	position.id = glGetUniformLocation(shader->id, position.name.c_str());
	screenSize.id = glGetUniformLocation(shader->id, screenSize.name.c_str());
	power.id = glGetUniformLocation(shader->id, power.name.c_str());
	frame.id = glGetUniformLocation(shader->id, frame.name.c_str());
	zoom.id = glGetUniformLocation(shader->id, zoom.name.c_str());
	mousePosition.id = glGetUniformLocation(shader->id, mousePosition.name.c_str());
	time.id = glGetUniformLocation(shader->id, time.name.c_str());
	GlErrorCheck();
}

void Fractal2D::SetUniformNames()
{
	position.name = "position";
	power.name = "power";
	screenSize.name = "screenSize";
	zoom.name = "zoom";
	mousePosition.name = "mousePosition";
	time.name = "time";
	frame.name = "frame";
}

void Fractal2D::SaveImage(const std::string path)
{
	renderShader->use();
	SetUniformLocations(renderShader);
	SetUniforms(renderShader);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



	Pixel* data = (Pixel*)malloc(screenSize.value.x * screenSize.value.y * sizeof(Pixel));
	glReadPixels(0, 0, screenSize.value.x, screenSize.value.y, GL_RGBA, GL_UNSIGNED_BYTE, data);


	explorationShader->use();
	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();

	Image image(screenSize.value.x, screenSize.value.y, data);

	image.FlipVertically();

	try
	{
		image.Save(path.c_str());
		DebugPrint("Successfully saved image \"" + FileManager::GetFileName(path) + "\"");
	}
	catch (const std::exception& e)
	{
		DebugPrint("Error saving image: " + *e.what());
		return;
	}
}

void Fractal2D::FindPathAndSaveImage()
{
	const std::string baseName = "TestImage/image";
	int count = 0;
	while (1)
	{
		if (!FileManager::FileExists((baseName + std::to_string(count) + ".png")))
		{
			SaveImage(baseName + std::to_string(count) + ".png");
			return;
		}
		count++;
	}
}

void Fractal2D::SetVariable(std::string name, std::string value)
{
	if (name == "power")
	{
		power.value = std::stof(value);
	}
	else if (name == "position")
	{
		std::vector<std::string> components = Split(value, ',');
		position.value = glm::vec2(std::stof(components[0]), std::stof(components[1]));
	}
	else if (name == "mousePosition")
	{
		std::vector<std::string> components = Split(value, ',');
		mousePosition.value = glm::vec2(std::stof(components[0]), std::stof(components[1]));
	}
}

void Fractal2D::SetVariablesFromSpec(int* index, std::string SpecificationPath)
{
	std::string specSection = GetSpecificationByIndex(&FileManager::ReadFile(SpecificationPath), index, FileManager::ReadFile(presetSpec2D));
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

void Fractal2D::HandleKeyInput()
{
	for (auto const& key : keys)
	{
		if (key.second)
		{
			switch (key.first)
			{
				// WASD movement
			case GLFW_KEY_W:
				position.value.y += static_cast<float>(time.value.GetDeltaTime() * parameterChangeRate * zoom.value);
				explorationShader->SetUniform(position);
				break;
			case GLFW_KEY_S:
				position.value.y -= static_cast<float>(time.value.GetDeltaTime() * parameterChangeRate * zoom.value);
				explorationShader->SetUniform(position);
				break;
			case GLFW_KEY_A:
				position.value.x -= static_cast<float>(time.value.GetDeltaTime() * parameterChangeRate * zoom.value);
				explorationShader->SetUniform(position);
				break;
			case GLFW_KEY_D:
				position.value.x += static_cast<float>(time.value.GetDeltaTime() * parameterChangeRate * zoom.value);
				explorationShader->SetUniform(position);
				break;

				// Variable change rate
			case GLFW_KEY_G:
				parameterChangeRate += 0.5f * static_cast<float>(time.value.GetDeltaTime());
				parameterChangeRate = std::max(parameterChangeRate, 0.01f);
				break;
			case GLFW_KEY_T:
				parameterChangeRate -= 0.5f * static_cast<float>(time.value.GetDeltaTime());
				parameterChangeRate = std::max(parameterChangeRate, 0.01f);
				break;

				// Zooming using exponential decay
			case GLFW_KEY_Q:
				zoom.value *= static_cast<float>(exp(time.value.GetDeltaTime() * -parameterChangeRate));
				explorationShader->SetUniform(zoom);
				break;
			case GLFW_KEY_E:
				zoom.value /= static_cast<float>(exp(time.value.GetDeltaTime() * -parameterChangeRate));
				explorationShader->SetUniform(zoom);
				break;

				// Changing the power of the fractal
			case GLFW_KEY_C:
				power.value += 0.5f * parameterChangeRate * static_cast<float>(time.value.GetDeltaTime());
				explorationShader->SetUniform(power);
				break;
			case GLFW_KEY_V:
				power.value -= 0.5f * parameterChangeRate * static_cast<float>(time.value.GetDeltaTime());
				explorationShader->SetUniform(power);
				break;


			case GLFW_KEY_R:
				if (explorationShader->type == compute)
				{
					renderShader->use();
					SetUniformLocations(renderShader);
					SetUniforms(renderShader);

					ComputeShader* explShader = reinterpret_cast<ComputeShader*>(explorationShader);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderShader->buffers[Fractal::rectangleVertexBufferIndexName].id);
					glBindVertexArray(renderShader->buffers[Fractal::rectangleVertexBufferName].id);
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderShader->buffers[computeRenderBufferName].id);
					glBindBufferBase(GL_SHADER_STORAGE_BUFFER, renderShader->buffers[computeRenderBufferName].binding, explShader->mainBuffer.id);
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

					explorationShader->use();
					SetUniformLocations(explorationShader);
					SetUniforms(explorationShader);
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, explShader->mainBuffer.id);
					glBindBufferBase(GL_SHADER_STORAGE_BUFFER, explShader->mainBuffer.binding, explShader->mainBuffer.id);
				}
				break;

			default:
				break;
			}
		}
	}
}

std::pair<Shader*, Shader*> Fractal2D::GenerateShader(int* specIndex, int* fractalIndex, std::string name)
{
	GlErrorCheck();

	std::vector<ShaderSection> sections{};

	std::string source = FileManager::ReadFile(Fractal2D::GetFractalPath(name));

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
			else if (value.size() == 3) sections.push_back(ShaderSection(value[0], StringToBool(value[1]), value[2]));
			else if (value.size() == 4) sections.push_back(ShaderSection(value[0], StringToBool(value[1]), value[2], StringToBool(value[3])));
		}
	}

	std::string highQualitySource;

	std::string renderSource = GetSection(Section("render"), source);
	if (renderSource != "")
	{
		highQualitySource = FileManager::ReadFile("shaders/2d/fractals/" + renderSource);
	}
	else
	{
		highQualitySource = std::string(source);
	}

	const std::string specification = FileManager::ReadFile(Fractal2D::GetSpecPath(name));



	return std::pair<Shader*, Shader*>(CreateShader(source, &specification, false, fractalIndex, specIndex, sections),
									   CreateShader(highQualitySource, &specification, true, fractalIndex, specIndex, sections));
	
}

std::pair<Shader*, Shader*> Fractal2D::GenerateShader()
{
	return GenerateShader(&specIndex, &fractalIndex, fractalName);
}

std::pair<Shader*, Shader*> Fractal2D::GenerateShader(std::string fractalName)
{
	int* specIndex = new int(0);
	int* fractalIndex = new int(0);
	return Fractal2D::GenerateShader(specIndex, fractalIndex, fractalName);
	delete specIndex;
	delete fractalIndex;
}

std::pair<Shader*, Shader*> Fractal2D::GenerateShader(int spec, int fractal, std::string fractalName)
{
	int* specIndex = new int(spec);
	int* fractalIndex = new int(fractal);
	return Fractal2D::GenerateShader(specIndex, fractalIndex, fractalName);
	delete specIndex;
	delete fractalIndex;
}

std::string Fractal2D::GetSpecPath(std::string fileName)
{
	return (Fractal2D::fractal2dPath + fileName) + "Specs.fs";
}

std::string Fractal2D::GetFractalPath(std::string fileName)
{
	return (Fractal2D::fractal2dPath + fileName) + ".fs";
}

std::string Fractal2D::GetFractalFolderPath()
{
	return fractal2dPath;
}

void Fractal2D::ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string & final, std::string specification, bool highQuality)
{
	// Bool in sections is for done or not 
	if (specification.find(Section("variables").start) != std::string::npos)
	{
		std::vector<std::string> variables = SplitNotInChar(GetSection(Section("variables"), specification), ',', '<', '>');
		for (size_t i = 0; i < variables.size(); i++)
		{

			std::string sectionName = GetSectionName(variables[i]);
			Section c = Section(sectionName);
			std::string from = c.start + GetSection(c, source) + c.end;

			if (!Replace(source, from, variables[i]))
			{
				DebugPrint("Couldn't replace variable " + sectionName + " from specification");
			}
		}
	}

	std::string defaultSource = default2DSource;

	const static std::string alternateDefaultFunctions = FileManager::ReadFile(alternateDefaultFunctionsPath);
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
			Section s("");

			if (highQuality && x.first.releaseName != "")
			{
				s = Section(x.first.releaseName);
			}
			else
			{
				s = Section(x.first.name);
			}

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

	// Constants
	const static size_t constSize = std::extent<decltype(constants2D)>::value;
	for (size_t i = 0; i < constSize; i++)
	{
		Section s("");
		ShaderSection c = constants2D[i];

		std::string name = (highQuality && c.releaseName != "") ? (c.releaseName) : (c.name);

		s = Section(name);

		std::string section = GetSection(s, source);
		if (section == "")
		{
			if ((section = GetSection(s, defaultSource)) == "")
			{
				// Default to normal name if a different one for release doesn't exist
				s = Section(c.name);

				if ((section = GetSection(s, source)) == "")
				{
					section = GetSection(s, defaultSource);
				}
			}
		}

		if (c.releaseName != "")
		{
			while (Replace(final, Section(c.name).start, section)) {}
		}
		else
		{
			while (Replace(final, s.start, section)) {}
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


	Section help("helperFunctions");
	std::string functions = GetSection(help, source);


	Section include("include");
	std::string includes = GetSection(include, source);

	if (includes != "")
	{
		includes.erase(std::remove(includes.begin(), includes.end(), '\n'), includes.end());
		includes.erase(std::remove(includes.begin(), includes.end(), '\t'), includes.end());
		includes.erase(std::remove(includes.begin(), includes.end(), ' '), includes.end());

		std::vector<std::string> includeList = Fractal::Split(includes, ',');

		std::string helperFunctions = FileManager::ReadFile(Fractal2D::helperFunctions);

		for (size_t i = 0; i < includeList.size(); i++)
		{
			functions += GetSection(Section(includeList[i]), helperFunctions);
		}
	}

	Replace(final, help.start, functions);


	std::string flags = GetSection(Section("flags"), specification);

	// Do this last, various reasons
	const static size_t postShaderSize = std::extent<decltype(postShaderSections2D)>::value;
	for (size_t i = 0; i < postShaderSize; i++)
	{
		Section s("");
		ShaderSection c = postShaderSections2D[i];
		if (c.optional && (source.find("<" + c.name + "Off>") != std::string::npos || flags.find("<" + c.name + "Off>") != std::string::npos))
		{
			Replace(final, Section(c.name).start, "");
			continue;
		}

		s = (highQuality && c.releaseName != "") ? Section(c.releaseName) : Section(c.name);

		std::string sectionString;
		if (c.multiple && source.find(s.start) != std::string::npos)
		{
			std::vector<std::string> versions;

			versions = SplitNotInChar(GetSection(s, (source.find(s.start) == std::string::npos) ? defaultSource : source), ',', '<', '>');
			std::string index = GetSection(s, specification);

			if (shaderIndices.size() != 0 && shaderIndices.count(c.name))
			{
				int* indexPtr = shaderIndices[c.name];
				if (*indexPtr < 0)* indexPtr = 0;
				else if ((size_t)* indexPtr > versions.size() - 1)* indexPtr = versions.size() - 1;
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

		if (c.releaseName != "")
		{
			while (Replace(final, Section(c.name).start, sectionString)) {}
		}
		else
		{
			while (Replace(final, s.start, sectionString)) {}
		}
	}
}

void Fractal2D::ParseShader(std::string& source, std::string & final, const std::string* spec, bool highQuality, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections)
{
	std::map<ShaderSection, bool> sections = std::map<ShaderSection, bool>();

	std::string specSection = GetSpecificationByIndex(spec, specIndex, FileManager::ReadFile(presetSpec2D));
	if (specSection == "")
	{
		DebugPrint("Specification error");
		return;
	}

	std::string tip = GetSection(Section("tip"), specSection);
	if (tip != "" && highQuality) // Only print once
	{
		size_t start = tip.find("\"") + 1;
		size_t end = tip.find_last_of("\"");
		std::cout << tip.substr(start, end - start) << std::endl;
	}

	if (source.find(Section("mainLoopOff").start) == std::string::npos)
	{
		BuildMainLoop(Section("mainLoop"), source, default2DSource, final, specSection, fractalIndex);
	}
	else
	{
		Replace(final, Section("mainLoop").start, "");
	}

	std::string flags = GetSection(Section("flags"), specSection);

	const static size_t sectionSize = std::extent<decltype(shaderSections2D)>::value;
	for (size_t i = 0; i < sectionSize; i++)
	{
		ShaderSection c = shaderSections2D[i];
		sections[c] = false;

		Section s = (highQuality && c.releaseName != "") ? Section(c.releaseName) : Section(c.name);

		if (flags.find("<" + GetSectionName(s.start) + "Default>") == std::string::npos)
		{
			sections[c] = ReplaceSection(s, Section(c.name), source, final);
		}
	}




	ParseShaderDefault(sections, source, final, specSection, highQuality);

	for (size_t i = 0; i < extraSections.size(); i++)
	{
		ShaderSection c = extraSections[i];

		Section s = (highQuality && c.releaseName != "") ? Section(c.releaseName) : Section(c.name);

		std::string sectionString;
		if (c.multiple && source.find(s.start) != std::string::npos)
		{
			std::vector<std::string> versions;

			versions = (source.find(s.start) == std::string::npos) ? SplitNotInChar(GetSection(s, default2DSource), ',', '<', '>') : SplitNotInChar(GetSection(s, source), ',', '<', '>');
			std::string index = GetSection(s, specSection);

			sectionString = versions[std::stoi(index)];

			CleanString(sectionString, { '<', '>' });

			while (Replace(final, s.start, sectionString)) {}
		}
		else
		{
			sectionString = (source.find(s.start) == std::string::npos) ? GetSection(s, source) : GetSection(s, source);
			while (Replace(final, Section(c.name).start, sectionString)) {}
		}
	}
}

void Fractal2D::Init()
{
	frame.value = 0;

	SetFractalNameFromIndex(&fractalNameIndex, GetFractalFolderPath());
	Fractal::fractalType = FractalType::fractal2D;
	SetVariablesFromSpec(&specIndex, GetSpecPath(fractalName));
	SetUniformNames();

	SetUniformLocations(renderShader);
	SetUniforms(renderShader);

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	explorationShader->use();
	GlErrorCheck();
}

std::map<std::string, int*> Fractal2D::GetDefaultShaderIndices()
{
	return {};
}

Shader* Fractal2D::CreateShader(std::string source, const std::string* specification, bool highQuality, int* fractalIndex, int* specIndex, std::vector<ShaderSection> shaderSections)
{
	const static std::string vertexSource = FileManager::ReadFile(Fractal::pathRectangleVertexshader);

	std::string base;
	std::string type = GetSection(Section("type"), source);
	if (type != "")
	{
		if (type == "fragment")
		{
			base = FileManager::ReadFile(Fractal2D::path2DBase);
		}
		else if (type == "compute")
		{
			const static int maxDimensions = 3;

			base = FileManager::ReadFile(Fractal2D::path2DBaseCompute);

			std::string dimensionNumber = GetSection(Section("localSizeDimensions"), source);
			if (dimensionNumber == "") dimensionNumber = "1";

			int dimensions;
			dimensions = std::stoi(dimensionNumber);
			if (dimensions > maxDimensions) dimensions = maxDimensions;

			int workGroupMaxProduct;
			glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupMaxProduct);

			int maxProductRoot = (int)std::floor(pow((double)workGroupMaxProduct, 1. / dimensions));

			int workGroups[maxDimensions] = { 1, 1, 1 };
			for (int i = 0; i < dimensions; i++)
			{
				workGroups[i] = maxProductRoot;
				int maxGroupSize;
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, i, &maxGroupSize);
				if (workGroups[i] > maxGroupSize)
				{
					workGroups[i] = 1;
				}
			}

			if (!Replace(base, "<sizeX>", std::to_string(workGroups[0]))) DebugPrint("Could not replace compute shader <sizeX> with " + std::to_string(workGroups[0]));
			if (!Replace(base, "<sizeY>", std::to_string(workGroups[1]))) DebugPrint("Could not replace compute shader <sizeY> with " + std::to_string(workGroups[1]));
			if (!Replace(base, "<sizeZ>", std::to_string(workGroups[2]))) DebugPrint("Could not replace compute shader <sizeZ> with " + std::to_string(workGroups[2]));

			if (source.find("std430" != 0))
			{
				Replace(base, "#version 330", "#version 430");
			}

			ParseShader(source, base, specification, highQuality, specIndex, fractalIndex, shaderSections);

			return new ComputeShader(base, false, { workGroups[0], workGroups[1], workGroups[2] });
		}
	}
	else // Assume fragment shader
	{
		base = FileManager::ReadFile(Fractal2D::path2DBase);
	}

	if (source.find("std430" != 0))
	{
		Replace(base, "#version 330", "#version 430");
	}
	ParseShader(source, base, specification, highQuality, specIndex, fractalIndex, shaderSections);

	return new Shader(vertexSource, base, false);

}
