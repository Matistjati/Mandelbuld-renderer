#include "headers/FileManager.h"
#include "headers/Fractal3D.h"
#include "headers/Image.h"
#include "headers/Debug.h"
#include <map>
#include <algorithm>
#include <thread>

const std::string& Fractal3D::default3DSource = FileManager::ReadFile(default3DFractal);

#define PrintSource 1

Fractal3D::Fractal3D(float power, Shader* explorationShader, Shader* renderShader, Camera& camera, glm::vec3 sun, glm::ivec2 screenSize, Time time, int* specIndex, std::string specification)
	: Fractal({ explorationShader, renderShader }, screenSize, time, GetDefaultShaderIndices()), camera(camera), sun(sun), power(power), genericParameter(1), cursorVisible(false)
{
	Init();
}

Fractal3D::Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex, glm::ivec2 screenSize)
	: Fractal(GenerateShader(specIndex, fractalIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()))[fractalNameIndex]), screenSize, Time(), GetDefaultShaderIndices(), 1.f, FractalType::fractal3D, fractalIndex, specIndex,
		fractalNameIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()))[fractalNameIndex]),
	camera(DefaultCamera),
	sun(glm::normalize(glm::vec3(0.577, 0.577, 0.577))),
	power(1), genericParameter(1), cursorVisible(false)
{
	Init();
}

Fractal3D::Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex)
	: Fractal(GenerateShader(GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()))[fractalNameIndex]), GetMonitorSize(), Time(), GetDefaultShaderIndices(), 1.f, FractalType::fractal3D, fractalIndex, specIndex,
		fractalNameIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()))[fractalNameIndex]),
	camera(DefaultCamera), 
	sun(glm::normalize(glm::vec3(0.577, 0.577, 0.577))),
	power(1), genericParameter(1), cursorVisible(false)
{
	Init();
}

void Fractal3D::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

		case GLFW_KEY_Q:
			(*shaderIndices["coloring"])++;
			update = true;
			break;
		case GLFW_KEY_A:
			(*shaderIndices["coloring"])--;
			update = true;
			break;

		case GLFW_KEY_W:
			(*shaderIndices["distanceSetup"])++;
			update = true;
			break;
		case GLFW_KEY_S:
			(*shaderIndices["distanceSetup"])--;
			update = true;
			break;

		case GLFW_KEY_E:
			(*shaderIndices["distanceExtraOperations"])++;
			update = true;
			break;
		case GLFW_KEY_D:
			(*shaderIndices["distanceExtraOperations"])--;
			update = true;
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
		default:
			break;
		case GLFW_KEY_Z:
			// Flipping the world
			camera.worldFlip.value *= -1;
			explorationShader->SetUniform(camera.worldFlip);
			break;
		case GLFW_KEY_X:
			time.value.ToogleTimePause();
			explorationShader->SetUniform(time);
			break;
		case GLFW_KEY_F:
			cursorVisible = !cursorVisible;
			if (cursorVisible)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			break;
		}
	}
}


void Fractal3D::MouseCallback(GLFWwindow* window, double x, double y)
{
	// If cursor is not set to first person, do not move
	if (cursorVisible)
	{
		return;
	}

	glm::vec2 newPos = glm::vec2(x, y);
	if (firstMouse)
	{
		mouseOffset = newPos;
		firstMouse = false;
	}
	Camera* c = &this->Fractal3D::camera;
	camera.ProcessMouseMovement(glm::vec2(newPos.x - mouseOffset.x, mouseOffset.y - newPos.y) * camera.mouseSensitivity * (1/zoom.value)); // reversed since y-coordinates go from bottom to top

	mouseOffset = newPos;

	explorationShader->SetUniform(camera.GetRotationMatrix());
}

void Fractal3D::MousePressCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		holdingMouse = true;
	}
	else if (action == GLFW_RELEASE)
	{
		holdingMouse = false;
	}
}

void Fractal3D::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	screenSize.value = glm::ivec2(width, height);
	explorationShader->SetUniform(screenSize);

	glViewport(0, 0, width, height);
}

void Fractal3D::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	zoom.value *= static_cast<float>(yoffset * time.value.GetDeltaTime() * scrollSpeed + 1);
	zoom.value = glm::max(1.0f, zoom.value);
	zoom.SetGuiValue();
	explorationShader->SetUniform(Uniform<float>(GetZoom(), zoom.id));
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
	shader->SetUniform(Uniform<float>(GetZoom(), zoom.id));
	shader->SetUniform(frame);
	shader->SetUniform(time);
	GlErrorCheck();
}

void Fractal3D::SetUniformLocations(Shader* shader)
{
	shader->use();
	camera.GetRotationMatrix().id = glGetUniformLocation(shader->id, camera.GetRotationMatrix().name.c_str());
	camera.position.id = glGetUniformLocation(shader->id, camera.position.name.c_str());
	camera.worldFlip.id = glGetUniformLocation(shader->id, camera.worldFlip.name.c_str());
	screenSize.id = glGetUniformLocation(shader->id, screenSize.name.c_str());
	sun.id = glGetUniformLocation(shader->id, sun.name.c_str());
	power.id = glGetUniformLocation(shader->id, power.name.c_str());
	genericParameter.id = glGetUniformLocation(shader->id, genericParameter.name.c_str());
	frame.id = glGetUniformLocation(shader->id, frame.name.c_str());
	zoom.id = glGetUniformLocation(shader->id, zoom.name.c_str());
	time.id = glGetUniformLocation(shader->id, time.name.c_str());
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
	frame.name = "frame";
	time.name = "time";
}

void Fractal3D::SaveImage(const std::string path)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, explorationShader->buffers[Fractal::rectangleVertexBufferIndexName].id);
	glBindVertexArray(explorationShader->buffers[Fractal::rectangleVertexBufferName].id);
	renderShader->use();
	SetUniformLocations(renderShader);
	SetUniforms(renderShader);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	
	std::vector<Pixel> data = std::vector<Pixel>(screenSize.value.x * screenSize.value.y);
	glReadPixels(0, 0, screenSize.value.x, screenSize.value.y, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);


	explorationShader->use();
	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();

	Image image(screenSize.value.x, screenSize.value.y, &data);

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

void Fractal3D::FindPathAndSaveImage()
{
	const static std::string baseName = "TestImage/image";
	int count = 0;
	// Finding the first unused file with name-pattern imageN.png where n is the number ascending
	while (FileManager::FileExists((baseName + std::to_string(count) + ".png"))) count++;

	SaveImage(baseName + std::to_string(count) + ".png");
}

void Fractal3D::PopulateGUI()
{
	Fractal::PopulateGUI();

	Fractal::PopulateGuiFromShader();

	gui->performLayout();
}

void Fractal3D::Update()
{
	Fractal::Update();

	// Move sun in shader
	double t = time.value.GetTotalTime();
	sun.value = glm::normalize(glm::vec3(sin(t * 0.25),
		std::abs(sin(t * 0.1)) * -1,
		cos(t * 0.25)));
	explorationShader->SetUniform(sun);
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

			if (shaderIndices.size() != 0 && shaderIndices.count(c.name))
			{
				if (versions[versions.size() - 1][0] != '<') versions.pop_back();

				int* indexPtr = shaderIndices[c.name];
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

		if (c.releaseName != "")
		{
			while (Replace(final, Section(c.name).start, sectionString)) {}
		}
		else
		{
			while(Replace(final, s.start, sectionString)) {}
		}
	}

	if (specification.find(Section("variables").start) != std::string::npos)
	{
		std::vector<std::string> variables = SplitNotInChar(GetSection(Section("variables"), specification), ',', { {'<', '>'}, {'(', ')' } });
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

				std::vector<std::string> uniformParts = SplitNotInChar(uniform, ' ', '(', ')');

				Replace(final, uniformParts[2], value, start);
			}
		}
	}
}

void Fractal3D::ParseShader(std::string& source, std::string& final, const std::string* spec, bool highQuality, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections)
{
	std::map<ShaderSection, bool> sections = std::map<ShaderSection, bool>();

	std::string specSection = GetSpecificationByIndex(spec, specIndex, FileManager::ReadFile(presetSpec3D));
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

	BuildMainLoop(Section("distanceEstimator"),	source, default3DSource, final, specSection, fractalIndex, shaderIndices.size() == 0 ? GetDefaultShaderIndices() : shaderIndices);

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
	frame.value = 0;

	SetFractalNameFromIndex(&fractalNameIndex, GetFractalFolderPath());
	Fractal::fractalType = FractalType::fractal3D;
	SetVariablesFromSpec(&specIndex, GetSpecPath(fractalName));
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	explorationShader->use();
	GlErrorCheck();

	PopulateGUI();
}

std::map<std::string, int*> Fractal3D::GetDefaultShaderIndices()
{
	return { {"coloring", new int(0)}, {"distanceSetup", new int(0)}, {"distanceExtraOperations", new int(0)}, };
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
	std::string specSection = GetSpecificationByIndex(&FileManager::ReadFile(SpecificationPath), index, FileManager::ReadFile(presetSpec3D));
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
	Fractal::HandleKeyInput();

	for (auto const& key : keys)
	{
		if (key.second)
		{
			switch (key.first)
			{
				// WASD movement
			case GLFW_KEY_W:
				camera.ProcessMovement(Camera_Movement::forward, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * GetZoom());
				explorationShader->SetUniform(camera.position);
				break;
			case GLFW_KEY_S:
				camera.ProcessMovement(Camera_Movement::back, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * GetZoom());
				explorationShader->SetUniform(camera.position);
				break;
			case GLFW_KEY_A:
				camera.ProcessMovement(Camera_Movement::left, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * GetZoom());
				explorationShader->SetUniform(camera.position);
				break;
			case GLFW_KEY_D:
				camera.ProcessMovement(Camera_Movement::right, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * GetZoom());
				explorationShader->SetUniform(camera.position);
				break;

				// Up and down
			case GLFW_KEY_SPACE:
				camera.ProcessMovement(Camera_Movement::up, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * GetZoom());
				explorationShader->SetUniform(camera.position);
				break;
			case GLFW_KEY_LEFT_SHIFT:
				camera.ProcessMovement(Camera_Movement::down, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate * GetZoom());
				explorationShader->SetUniform(camera.position);
				break;

				// Camera roll
			case GLFW_KEY_Q:
				camera.ProcessRoll(static_cast<float>(camera.rollSpeed * time.value.GetDeltaTime() * parameterChangeRate * GetZoom()));
				explorationShader->SetUniform(camera.GetRotationMatrix());
				break;
			case GLFW_KEY_E:
				camera.ProcessRoll(-static_cast<float>(camera.rollSpeed * time.value.GetDeltaTime() * parameterChangeRate * GetZoom()));
				explorationShader->SetUniform(camera.GetRotationMatrix());
				break;

				// Changing the power of the fractal
			case GLFW_KEY_C:
				power.value += 0.5f * parameterChangeRate * static_cast<float>(time.value.GetDeltaTime()) * GetZoom();
				explorationShader->SetUniform(power);
				break;
			case GLFW_KEY_V:
				power.value -= 0.5f * parameterChangeRate * static_cast<float>(time.value.GetDeltaTime()) * GetZoom();
				explorationShader->SetUniform(power);
				break;

			case GLFW_KEY_R:
				genericParameter.value += 0.1f * parameterChangeRate * static_cast<float>(time.value.GetDeltaTime()) * GetZoom();
				explorationShader->SetUniform(genericParameter);
				break;
			case GLFW_KEY_F:
				genericParameter.value -= 0.1f * parameterChangeRate * static_cast<float>(time.value.GetDeltaTime()) * GetZoom();
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


	fractalSourceCode = baseCopy;

#if PrintSource
	std::cout << baseCopy;
#endif

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
