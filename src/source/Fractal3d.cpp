#include "headers/FileManager.h"
#include "headers/Fractal3d.h"
#include "headers/Image.h"
#include "headers/Debug.h"
#include <map>
#include <algorithm>
#include <thread>

const std::string& Fractal3D::default3DSource = FileManager::ReadFile(default3DFractal);

Fractal3D::Fractal3D(float power, Shader* explorationShader, Shader* renderShader, Camera& camera, glm::vec3 sun, glm::ivec2 screenSize, Time time, int* specIndex, std::string specification)
	: Fractal({ explorationShader, renderShader }, screenSize, time), camera(camera), sun(sun), power(power), genericParameter(1)
{
	Init();
}

Fractal3D::Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex, glm::ivec2 screenSize)
	: Fractal(GenerateShader(specIndex, fractalIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()))[fractalNameIndex]), screenSize, Time(), 1.f, fractal3D, fractalIndex, specIndex,
		fractalNameIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()))[fractalNameIndex]),
	camera(DefaultCamera),
	sun(glm::normalize(glm::vec3(0.577, 0.577, 0.577))),
	power(1), genericParameter(1)
{
	Init();
}

Fractal3D::Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex)
	: Fractal(GenerateShader(GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()))[fractalNameIndex]), GetMonitorSize(), Time(), 1.f, fractal3D, fractalIndex, specIndex, 
		fractalNameIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()))[fractalNameIndex]),
	camera(DefaultCamera), 
		sun(glm::normalize(glm::vec3(0.577, 0.577, 0.577))),
	power(1), genericParameter(1)
{
	Init();
}

void Fractal3D::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UNKNOWN) return; // Stay away from weird stuff

	// Handle input actions in separate function
	if ((action == GLFW_PRESS) && (mods & GLFW_MOD_CONTROL) != GLFW_MOD_CONTROL) keys[key] = true;
	else if ((action == GLFW_RELEASE) && (mods & GLFW_MOD_CONTROL) != GLFW_MOD_CONTROL) keys[key] = false;

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		break;

		// Ctrl key handling
	case GLFW_KEY_S:
		if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL && action == GLFW_PRESS)
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
		break;

	case GLFW_KEY_D:
		if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL && action == GLFW_PRESS)
			BreakIfDebug();
		break;

		// World stuff
	case GLFW_KEY_Z:
		if (action == GLFW_PRESS)
		{
			camera.worldFlip.value *= -1;
			explorationShader->SetUniform(camera.worldFlip);
		}
		break;
	}
}


void Fractal3D::MouseCallback(GLFWwindow* window, double x, double y)
{
	glm::vec2 newPos = glm::vec2(x, y);
	if (firstMouse)
	{
		mouseOffset = newPos;
		firstMouse = false;
	}
	Camera* c = &this->Fractal3D::camera;
	camera.ProcessMouseMovement(static_cast<float>(newPos.x - mouseOffset.x),
		static_cast<float>(mouseOffset.y - newPos.y) // reversed since y-coordinates go from bottom to top
	);

	mouseOffset = newPos;

	explorationShader->SetUniform(camera.GetRotationMatrix());
}

void Fractal3D::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	screenSize.value = glm::ivec2(width, height);
	explorationShader->SetUniform(screenSize);

	glViewport(0, 0, width, height);
}

void Fractal3D::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (zoom.value + yoffset < 1)
	{
		zoom.value = 1;
	}
	else
	{
		// This works surprisingly well
		zoom.value += static_cast<float>(yoffset * time.value.deltaTime * scrollSpeed * zoom.value);
	}
	explorationShader->SetUniform(zoom);
}

void Fractal3D::SetUniforms(Shader* shader)
{
	shader->use();
	shader->SetUniform(camera.position);
	shader->SetUniform(camera.GetRotationMatrix());
	shader->SetUniform(camera.worldFlip);
	shader->SetUniform(screenSize);
	shader->SetUniform(sun);
	shader->SetUniform(power);
	shader->SetUniform(genericParameter);
	explorationShader->SetUniform(zoom);
	GlErrorCheck();
}

void Fractal3D::SetUniformLocations(Shader* shader)
{
	camera.GetRotationMatrix().id = glGetUniformLocation(shader->id, camera.GetRotationMatrix().name.c_str());
	camera.position.id = glGetUniformLocation(shader->id, camera.position.name.c_str());
	camera.worldFlip.id = glGetUniformLocation(shader->id, camera.worldFlip.name.c_str());
	screenSize.id = glGetUniformLocation(shader->id, screenSize.name.c_str());
	sun.id = glGetUniformLocation(shader->id, sun.name.c_str());
	power.id = glGetUniformLocation(shader->id, power.name.c_str());
	genericParameter.id = glGetUniformLocation(shader->id, genericParameter.name.c_str());
	zoom.id = glGetUniformLocation(shader->id, zoom.name.c_str());
	GlErrorCheck();
}

void Fractal3D::SetUniformNames()
{
	camera.GetRotationMatrix().name = "rotation";
	camera.position.name = "position";
	camera.worldFlip.name = "worldFlip";
	screenSize.name = "screenSize";
	sun.name = "sun";
	power.name = "power";
	genericParameter.name = "genericParameter";
	zoom.name = "zoom";
}

void Fractal3D::SaveImage(const std::string path)
{
	renderShader->use();
	SetUniformLocations(renderShader);
	SetUniforms(renderShader);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	
	Pixel* data = (Pixel*)malloc(screenSize.value.x * screenSize.value.y * 4);
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

void Fractal3D::Update()
{
	// Move sun in shader
	double t = time.value.GetTotalTime();
	sun.value = glm::normalize(glm::vec3(sin(t * 0.25),
		std::abs(sin(t * 0.1)) * -1,
		cos(t * 0.25)));

	explorationShader->SetUniform(sun);

	Fractal::Update();
}

void Fractal3D::ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string& final, std::string specification, bool highQuality)
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
				DebugPrint("Could not replace variable from specification");
			}
		}
	}

	std::string defaultSource = default3DSource;

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
	const static size_t constSize = std::extent<decltype(constants)>::value;
	for (size_t i = 0; i < constSize; i++)
	{
		Section s("");
		ShaderSection c = constants[i];

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

		std::string helperFunctions = FileManager::ReadFile(Fractal3D::helperFunctions);
		
		for (size_t i = 0; i < includeList.size(); i++)
		{
			functions += GetSection(Section(includeList[i]), helperFunctions);
		}
	}

	Replace(final, help.start, functions);


	std::string flags = GetSection(Section("flags"), specification);

	// Do this last, various reasons
	const static size_t postShaderSize = std::extent<decltype(postShaderSections)>::value;
	for (size_t i = 0; i < postShaderSize; i++)
	{
		Section s("");
		ShaderSection c = postShaderSections[i];
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
			if (index == "")
			{
				Replace(final, s.start, "");
				continue;
			}
			size_t indexInt = std::stoi(index);
			if (indexInt > versions.size() - 1)
			{
				DebugPrint("Index was " + std::to_string(indexInt) + ", which is out of bounds at " + c.name);
				BreakIfDebug();
				continue;
			}
			sectionString = versions[indexInt];

			CleanString(sectionString, { '<','>' });
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
			while(Replace(final, s.start, sectionString)) {}
		}
	}
}

void Fractal3D::ParseShader(std::string& source, std::string& final, const std::string* spec, bool highQuality, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections)
{
	std::map<ShaderSection, bool> sections = std::map<ShaderSection, bool>();

	std::string specSection = GetSpecificationByIndex(spec, specIndex, FileManager::ReadFile(presetSpec));
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

	BuildDistanceEstimator(source, default3DSource, final, specSection, fractalIndex);

	std::string flags = GetSection(Section("flags"), specSection);

	const static size_t sectionSize = std::extent<decltype(shaderSections)>::value;
	for (size_t i = 0; i < sectionSize; i++)
	{
		ShaderSection c = shaderSections[i];
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

			versions = (source.find(s.start) == std::string::npos) ? SplitNotInChar(GetSection(s, default3DSource), ',', '<', '>') : SplitNotInChar(GetSection(s, source), ',', '<', '>');
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

void Fractal3D::Init()
{
	SetVariablesFromSpec(&specIndex, GetSpecPath(fractalName));
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	explorationShader->use();
	GlErrorCheck();
}

// This is really nasty, be careful
inline void Fractal3D::SetVariable(std::string name, std::string value)
{
	if (name == "power")
	{
		power.value = std::stof(value);
	}
	else if (name == "genericParameter")
	{
		genericParameter.value = std::stof(value);
	}
	else if (name == "position")
	{
		std::vector<std::string> components = Split(value, ',');
		camera.position.value = glm::vec3(std::stof(components[0]), std::stof(components[1]), std::stof(components[2]));
	}
	else if (name == "yaw")
	{
		camera.SetYaw(std::stof(value));
	}
	else if (name == "pitch")
	{
		camera.SetPitch(std::stof(value));
	}
	else if (name == "roll")
	{
		camera.SetRoll(std::stof(value));
	}
	else if (name == "sun")
	{
		std::vector<std::string> components = Split(value, ',');
		sun.value = glm::vec3(std::stof(components[0]), std::stof(components[1]), std::stof(components[2]));
	}
	else if (name == "worldFlip")
	{
		camera.worldFlip.value = std::stoi(value);
	}
}

void Fractal3D::SetVariablesFromSpec(int* index, std::string SpecificationPath)
{
	std::string specSection = GetSpecificationByIndex(&FileManager::ReadFile(SpecificationPath), index, FileManager::ReadFile(presetSpec));
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

void Fractal3D::HandleKeyInput()
{
	for (auto const& key : keys)
	{
		if (key.second)
		{
			switch (key.first)
			{
				// WASD movement
			case GLFW_KEY_W:
				camera.ProcessMovement(Camera_Movement::forward, static_cast<float>(time.value.deltaTime) * parameterChangeRate);
				explorationShader->SetUniform(camera.position);
				break;
			case GLFW_KEY_S:
				camera.ProcessMovement(Camera_Movement::back, static_cast<float>(time.value.deltaTime) * parameterChangeRate);
				explorationShader->SetUniform(camera.position);
				break;
			case GLFW_KEY_A:
				camera.ProcessMovement(Camera_Movement::left, static_cast<float>(time.value.deltaTime) * parameterChangeRate);
				explorationShader->SetUniform(camera.position);
				break;
			case GLFW_KEY_D:
				camera.ProcessMovement(Camera_Movement::right, static_cast<float>(time.value.deltaTime) * parameterChangeRate);
				explorationShader->SetUniform(camera.position);
				break;

				// Up and down
			case GLFW_KEY_SPACE:
				camera.ProcessMovement(Camera_Movement::up, static_cast<float>(time.value.deltaTime) * parameterChangeRate);
				explorationShader->SetUniform(camera.position);
				break;
			case GLFW_KEY_LEFT_SHIFT:
				camera.ProcessMovement(Camera_Movement::down, static_cast<float>(time.value.deltaTime) * parameterChangeRate);
				explorationShader->SetUniform(camera.position);
				break;

				// Variable change rate
			case GLFW_KEY_G:
				parameterChangeRate += 0.5f * static_cast<float>(time.value.deltaTime);
				parameterChangeRate = std::max(parameterChangeRate, 0.01f);
				break;
			case GLFW_KEY_T:
				parameterChangeRate -= 0.5f * static_cast<float>(time.value.deltaTime);
				parameterChangeRate = std::max(parameterChangeRate, 0.01f);
				break;

				// Camera roll
			case GLFW_KEY_Q:
				camera.ProcessRoll(static_cast<float>(camera.rollSpeed * time.value.deltaTime * parameterChangeRate));
				explorationShader->SetUniform(camera.GetRotationMatrix());
				break;
			case GLFW_KEY_E:
				camera.ProcessRoll(-static_cast<float>(camera.rollSpeed * time.value.deltaTime * parameterChangeRate));
				explorationShader->SetUniform(camera.GetRotationMatrix());
				break;

				// Changing the power of the fractal
			case GLFW_KEY_C:
				power.value += 0.5f * parameterChangeRate * static_cast<float>(time.value.deltaTime);
				explorationShader->SetUniform(power);
				break;
			case GLFW_KEY_V:
				power.value -= 0.5f * parameterChangeRate * static_cast<float>(time.value.deltaTime);
				explorationShader->SetUniform(power);
				break;

			case GLFW_KEY_R:
				genericParameter.value += 0.1f * parameterChangeRate * static_cast<float>(time.value.deltaTime);
				explorationShader->SetUniform(genericParameter);
				break;
			case GLFW_KEY_F:
				genericParameter.value -= 0.1f * parameterChangeRate * static_cast<float>(time.value.deltaTime);
				explorationShader->SetUniform(genericParameter);
				break;


			default:
				break;
			}
		}
	}
}

std::pair<Shader*, Shader*> Fractal3D::GenerateShader(int* specIndex, int* fractalIndex, std::string name)
{
	GlErrorCheck();

	std::string base = FileManager::ReadFile(Fractal3D::path3DBase);

	std::vector<ShaderSection> sections{};

	std::string source = FileManager::ReadFile(Fractal3D::GetFractalPath(name));

	Section extraSects = Section("extraSections");
	size_t extraSectionIndex = source.find(extraSects.start);
	if (extraSectionIndex != std::string::npos)
	{
		size_t extraSectionEnd = source.find(extraSects.end);
		std::vector<std::string> sectionContents = SplitNotInChar(source.substr(extraSectionIndex + extraSects.start.length(), extraSectionEnd - (extraSectionIndex + extraSects.start.length())), ',', '[', ']');
		for (size_t i = 0; i < sectionContents.size(); i++)
		{
			CleanString(sectionContents[i], {'\n', '\t', ' ', '[', ']', '\"'});
			std::vector<std::string> value = Split(sectionContents[i], ',');
			if (value.size() == 1)	sections.push_back(ShaderSection(value[0]));
			else if (value.size() == 2) sections.push_back(ShaderSection(value[0], StringToBool(value[1])));
			else if (value.size() == 3) sections.push_back(ShaderSection(value[0], StringToBool(value[1]), value[2]));
			else if (value.size() == 4) sections.push_back(ShaderSection(value[0], StringToBool(value[1]), value[2], StringToBool(value[3])));
		}
	}

	const std::string specification = FileManager::ReadFile(Fractal3D::GetSpecPath(name));

	std::string sourceCopy = std::string(source);
	std::string baseCopy = std::string(base);
	ParseShader(sourceCopy, baseCopy, &specification, false, specIndex, fractalIndex, sections);

	ParseShader(source, base, &specification, true, specIndex, fractalIndex, sections);

	const static std::string vertexSource = FileManager::ReadFile(Fractal::pathRectangleVertexshader);

	return std::pair<Shader*, Shader*>((new Shader(vertexSource, baseCopy, false)),
									   (new Shader(vertexSource, base,     false)));
}

std::pair<Shader*, Shader*> Fractal3D::GenerateShader()
{
	return GenerateShader(&specIndex, &fractalIndex, fractalName);
}

std::pair<Shader*, Shader*> Fractal3D::GenerateShader(std::string fractalName)
{
	int* specIndex = new int(0);
	int* fractalIndex = new int(0);
	return Fractal3D::GenerateShader(specIndex, fractalIndex, fractalName);
	delete specIndex;
	delete fractalIndex;
}

std::pair<Shader*, Shader*> Fractal3D::GenerateShader(int spec, int fractal, std::string fractalName)
{
	int* specIndex = new int(spec);
	int* fractalIndex = new int(fractal);
	return Fractal3D::GenerateShader(specIndex, fractalIndex, fractalName);
	delete specIndex;
	delete fractalIndex;
}

std::string Fractal3D::GetSpecPath(std::string fileName)
{
	return (Fractal3D::fractal3dPath + fileName) + "Specs.fs";
}

std::string Fractal3D::GetFractalPath(std::string fileName)
{
	return (Fractal3D::fractal3dPath + fileName) + ".fs";
}

std::string Fractal3D::GetFractalFolderPath()
{
	return fractal3dPath;
}
