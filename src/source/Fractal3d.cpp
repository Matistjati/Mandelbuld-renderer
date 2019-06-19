#include "headers/FileManager.h"
#include "headers/Fractal3d.h"
#include "headers/Image.h"
#include "headers/Debug.h"
#include <map>
#include <algorithm>
#include <thread>

const std::string& Fractal3D::default3DSource = FileManager::readFile(default3DFractal);

Fractal3D::Fractal3D(float power, Shader& explorationShader, Shader& renderShader, Camera& camera, glm::vec3 sun, glm::ivec2 screenSize, Time time, int specIndex, std::string specification)
	: Fractal(explorationShader, renderShader, screenSize, time), camera(camera), sun(sun), power(power), genericParameter(1)
{
	Init(specIndex, specification);
}

Fractal3D::Fractal3D(int specIndex, std::string specification, std::string sourcePath)
	: Fractal(GenerateShader(false, specIndex, FileManager::readFile(specification), FileManager::readFile(sourcePath)), 
			  GenerateShader(true , specIndex, FileManager::readFile(specification), FileManager::readFile(sourcePath)), GetMonitorSize(), Time()),
	camera(*(new Camera(glm::vec3(1.8f, 0.8f, -0.6f), // Position
		169, -14, 0.001f, // Yaw, pitch, roll
		0.15f, 3, 200))), // mouseSensitivity, movementSpeed, rollSpeed
		sun(glm::normalize(glm::vec3(0.577, 0.577, 0.577))),
	power(1), genericParameter(1)
{
	Init(specIndex, specification);
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
				if (!FileManager::fileExists((baseName + std::to_string(count) + ".png")))
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
			__debugbreak();
		break;

		// World stuff
	case GLFW_KEY_Z:
		if (action == GLFW_PRESS)
		{
			camera.worldFlip.value *= -1;
			explorationShader.SetUniform(camera.worldFlip);
		}
		break;
	case GLFW_KEY_X:
		if (action == GLFW_PRESS)
		{
			camera.movementReverse *= -1;
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

	explorationShader.SetUniform(camera.GetRotationMatrix());
}

void Fractal3D::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	screenSize.value = glm::ivec2(width, height);
	explorationShader.SetUniform(screenSize);

	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
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
		zoom.value += static_cast<float>(yoffset*time.deltaTime*scrollSpeed*zoom.value);
	}
	explorationShader.SetUniform(zoom);
}

void Fractal3D::SetUniforms(Shader& shader)
{
	shader.use();
	shader.SetUniform(camera.position);
	shader.SetUniform(camera.GetRotationMatrix());
	shader.SetUniform(camera.worldFlip);
	shader.SetUniform(screenSize);
	shader.SetUniform(sun);
	shader.SetUniform(power);
	shader.SetUniform(genericParameter);
	explorationShader.SetUniform(zoom);
	GlErrorCheck();
}

void Fractal3D::SetUniformLocations(Shader& shader)
{
	camera.GetRotationMatrix().id = glGetUniformLocation(shader.id, camera.GetRotationMatrix().name.c_str());
	camera.position.id = glGetUniformLocation(shader.id, camera.position.name.c_str());
	camera.worldFlip.id = glGetUniformLocation(shader.id, camera.worldFlip.name.c_str());
	screenSize.id = glGetUniformLocation(shader.id, screenSize.name.c_str());
	sun.id = glGetUniformLocation(shader.id, sun.name.c_str());
	power.id = glGetUniformLocation(shader.id, power.name.c_str());
	genericParameter.id = glGetUniformLocation(shader.id, genericParameter.name.c_str());
	zoom.id = glGetUniformLocation(shader.id, zoom.name.c_str());
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
	renderShader.use();
	SetUniformLocations(renderShader);
	SetUniforms(renderShader);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	
	Pixel* data = (Pixel*)malloc(screenSize.value.x * screenSize.value.y * 4);
	glReadPixels(0, 0, screenSize.value.x, screenSize.value.y, GL_RGBA, GL_UNSIGNED_BYTE, data);


	explorationShader.use();
	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();

	Image image(screenSize.value.x, screenSize.value.y, data);

	image.FlipVertically();

	try
	{
		image.Save(path.c_str());
		DebugPrint("Successfully saved image \"" + FileManager::getFileName(path) + "\"");
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
	double t = glfwGetTime();
	sun.value = glm::normalize(glm::vec3(sin(t * 0.25),
		std::abs(sin(t * 0.1)) * -1,
		cos(t * 0.25)));

	explorationShader.SetUniform(sun);

	time.PollTime();
}

void Fractal3D::ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string& final, std::string specification, bool highQuality)
{
	// Bool in sections is for done or not 
	if (specification.find(Section("variables").start) != std::string::npos)
	{
		std::vector<std::string> variables = splitNotInChar(getSection(Section("variables"), specification), ',', '<', '>');
		for (size_t i = 0; i < variables.size(); i++)
		{
			
			std::string sectionName = getSectionName(variables[i]);
			Section c = Section(sectionName);
			std::string from = c.start + getSection(c, source) + c.end;

			if (!replace(source, from, variables[i]))
			{
				DebugPrint("Could not replace variable from specification");
			}
		}
	}

	std::string defaultSource = default3DSource;

	// Default code for sections not implemented
	for (auto const& x : sections)
	{
		if (!x.first.optional && !x.second) // Not done
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
			replaceSection(s, Section(x.first.name), defaultSource, final);
		}
		else
		{
			replace(final, Section(x.first.name).start, "");
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

		std::string section = getSection(s, source);
		if (section == "")
		{
			if ((section = getSection(s, defaultSource)) == "")
			{
				// Default to normal name if a different one for release doesn't exist
				s = Section(c.name);

				if ((section = getSection(s, source)) == "")
				{
					section = getSection(s, defaultSource);
				}
			}
		}

		if (c.releaseName != "")
		{
			while (replace(final, Section(c.name).start, section)) {}
		}
		else
		{
			while (replace(final, s.start, section)) {}
		}
	}


	Section help("helperFunctions");
	std::string functions = getSection(help, source);


	Section include("include");
	std::string includes = getSection(include, source);

	if (includes != "")
	{
		includes.erase(std::remove(includes.begin(), includes.end(), '\n'), includes.end());
		includes.erase(std::remove(includes.begin(), includes.end(), '\t'), includes.end());
		includes.erase(std::remove(includes.begin(), includes.end(), ' '), includes.end());

		std::vector<std::string> includeList = Fractal::split(includes, ',');

		std::string helperFunctions = FileManager::readFile(Fractal3D::helperFunctions);
		
		for (size_t i = 0; i < includeList.size(); i++)
		{
			functions += getSection(Section(includeList[i]), helperFunctions);
		}
	}

	replace(final, help.start, functions);


	std::string flags = getSection(Section("flags"), specification);

	// Do this last, various reasons
	const static size_t postShaderSize = std::extent<decltype(postShaderSections)>::value;
	for (size_t i = 0; i < postShaderSize; i++)
	{
		Section s("");
		ShaderSection c = postShaderSections[i];
		if (c.optional && (source.find("<" + c.name + "Off>") != std::string::npos || flags.find("<" + c.name + "Off>") != std::string::npos))
		{
			replace(final, Section(c.name).start, "");
			continue;
		}

		s = (highQuality && c.releaseName != "") ? Section(c.releaseName) : Section(c.name);
		
		std::string sectionString;
		if (c.multiple && source.find(s.start) != std::string::npos)
		{
			std::vector<std::string> versions;
			
			versions = splitNotInChar(getSection(s, (source.find(s.start) == std::string::npos) ? defaultSource : source), ',', '<', '>');
			std::string index = getSection(s, specification);
			if (index == "")
			{
				if (s.start.find("color") != std::string::npos)
				{
					index = "0";
				}
				else
				{
					replace(final, s.start, "");
					continue;
				}
			}
			size_t indexInt = std::stoi(index);
			if (indexInt > versions.size() - 1)
			{
				DebugPrint("Index was " + std::to_string(indexInt) + ", which is out of bounds at " + c.name);
				BreakIfDebug();
				continue;
			}
			sectionString = versions[indexInt];

			// Clean string
			sectionString.erase(std::remove(sectionString.begin(), sectionString.end(), '<'), sectionString.end());
			sectionString.erase(std::remove(sectionString.begin(), sectionString.end(), '>'), sectionString.end());
		}
		else
		{
			sectionString = (source.find(s.start) == std::string::npos) ? getSection(s, defaultSource) : getSection(s, source);
		}

		if (c.releaseName != "")
		{
			while (replace(final, Section(c.name).start, sectionString)) {}
		}
		else
		{
			while(replace(final, s.start, sectionString)) {}
		}
	}
}

void Fractal3D::ParseShader(std::string& source, std::string& final, std::string spec, bool highQuality, int specIndex, const std::vector<ShaderSection> extraSections)
{
	std::map<ShaderSection, bool> sections = std::map<ShaderSection, bool>();

	std::string specSection = GetSpecificationByIndex(spec, specIndex, FileManager::readFile(presetSpec));
	if (specSection == "")
	{
		DebugPrint("Specification error");
		return;
	}

	BuildDistanceEstimator(source, default3DSource, final, specSection);

	std::string flags = getSection(Section("flags"), specSection);

	const static size_t sectionSize = std::extent<decltype(shaderSections)>::value;
	for (size_t i = 0; i < sectionSize; i++)
	{
		ShaderSection c = shaderSections[i];
		sections[c] = false;

		Section s = (highQuality && c.releaseName != "") ? Section(c.releaseName) : Section(c.name);

		if (flags.find("<" + getSectionName(s.start) + "Default>") == std::string::npos)
		{
			sections[c] = replaceSection(s, Section(c.name), source, final);
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

			versions = (source.find(s.start) == std::string::npos) ? splitNotInChar(getSection(s, default3DSource), ',', '<', '>') : splitNotInChar(getSection(s, source), ',', '<', '>');
			std::string index = getSection(s, specSection);

			sectionString = versions[std::stoi(index)];
			// Clean string
			sectionString.erase(std::remove(sectionString.begin(), sectionString.end(), '<'), sectionString.end());
			sectionString.erase(std::remove(sectionString.begin(), sectionString.end(), '>'), sectionString.end());
			while (replace(final, s.start, sectionString)) {}
		}
		else
		{
			sectionString = (source.find(s.start) == std::string::npos) ? getSection(s, source) : getSection(s, source);
			while (replace(final, Section(c.name).start, sectionString)) {}
		}
	}
}

void Fractal3D::Init(int specIndex, std::string specification)
{
	SetVariablesFromSpec(specIndex, specification);
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
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
		std::vector<std::string> components = split(value, ',');
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
		std::vector<std::string> components = split(value, ',');
		sun.value = glm::vec3(std::stof(components[0]), std::stof(components[1]), std::stof(components[2]));
	}
	else if (name == "movementReverse")
	{
		camera.movementReverse = std::stof(value);
	}
	else if (name == "worldFlip")
	{
		camera.worldFlip.value = std::stoi(value);
	}
}

void Fractal3D::SetVariablesFromSpec(int index, std::string SpecificationPath)
{
	std::string specSection = GetSpecificationByIndex(FileManager::readFile(SpecificationPath), index, FileManager::readFile(presetSpec));
	std::string variables = getSection(Section("cpuVariables"), specSection);
	if (variables != "")
	{
		std::vector<std::string> variablesList = splitNotInChar(variables, ',', '[', ']');
		for (size_t i = 0; i < variablesList.size(); i++)
		{
			std::string value = getSectionValue(variablesList[i]);

			int indexStart = value.find('(');
			if (indexStart != std::string::npos)
			{
				int indexEnd = value.find(')', indexStart);
				if (indexEnd != std::string::npos)
				{
					size_t index = std::stoi(value.substr(indexStart + 1, indexEnd - 2));
					if (value[value.length() - 1] == ']' && value[value.length() - 2] == ']')
					{
						value.erase(value.length() - 1);
					}
					std::vector<std::string> values = splitNotInChar(value.substr(indexEnd + 1), ',', '[', ']');
					if (index > values.size() - 1)
					{
						DebugPrint("Index was too large: " + std::to_string(index) + " at " + getSectionName(variables));
						BreakIfDebug();
					}
					value = values[index];
				}
			}

			cleanString(value, { '[', ']' });
			if (value[0] == ',') value[0] = '\n'; // Leading comma breaks stuff
			SetVariable(getSectionName(variablesList[i]), value);
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
				camera.ProcessMovement(Camera_Movement::forward, static_cast<float>(time.deltaTime) * parameterChangeRate);
				explorationShader.SetUniform(camera.position);
				break;
			case GLFW_KEY_S:
				camera.ProcessMovement(Camera_Movement::back, static_cast<float>(time.deltaTime) * parameterChangeRate);
				explorationShader.SetUniform(camera.position);
				break;
			case GLFW_KEY_A:
				camera.ProcessMovement(Camera_Movement::left, static_cast<float>(time.deltaTime) * parameterChangeRate);
				explorationShader.SetUniform(camera.position);
				break;
			case GLFW_KEY_D:
				camera.ProcessMovement(Camera_Movement::right, static_cast<float>(time.deltaTime) * parameterChangeRate);
				explorationShader.SetUniform(camera.position);
				break;

				// Up and down
			case GLFW_KEY_SPACE:
				camera.ProcessMovement(Camera_Movement::up, static_cast<float>(time.deltaTime) * parameterChangeRate);
				explorationShader.SetUniform(camera.position);
				break;
			case GLFW_KEY_LEFT_SHIFT:
				camera.ProcessMovement(Camera_Movement::down, static_cast<float>(time.deltaTime) * parameterChangeRate);
				explorationShader.SetUniform(camera.position);
				break;

				// Variable change rate
			case GLFW_KEY_G:
				parameterChangeRate += 0.5f * static_cast<float>(time.deltaTime);
				parameterChangeRate = std::max(parameterChangeRate, 0.01f);
				break;
			case GLFW_KEY_T:
				parameterChangeRate -= 0.5f * static_cast<float>(time.deltaTime);
				parameterChangeRate = std::max(parameterChangeRate, 0.01f);
				break;

				// Camera roll
			case GLFW_KEY_Q:
				camera.ProcessRoll(static_cast<float>(camera.rollSpeed * time.deltaTime * parameterChangeRate));
				explorationShader.SetUniform(camera.GetRotationMatrix());
				break;
			case GLFW_KEY_E:
				camera.ProcessRoll(-static_cast<float>(camera.rollSpeed * time.deltaTime * parameterChangeRate));
				explorationShader.SetUniform(camera.GetRotationMatrix());
				break;

				// Changing the power of the fractal
			case GLFW_KEY_C:
				power.value += 0.5f * parameterChangeRate * static_cast<float>(time.deltaTime);
				explorationShader.SetUniform(power);
				break;
			case GLFW_KEY_V:
				power.value -= 0.5f * parameterChangeRate * static_cast<float>(time.deltaTime);
				explorationShader.SetUniform(power);
				break;

			case GLFW_KEY_R:
				genericParameter.value += 0.1f * parameterChangeRate * static_cast<float>(time.deltaTime);
				explorationShader.SetUniform(genericParameter);
				break;
			case GLFW_KEY_F:
				genericParameter.value -= 0.1f * parameterChangeRate * static_cast<float>(time.deltaTime);
				explorationShader.SetUniform(genericParameter);
				break;


			default:
				break;
			}
		}
	}
}

Shader& Fractal3D::GenerateShader(bool highQuality, int specIndex, std::string specification, std::string source)
{
	GlErrorCheck();

	std::string base = FileManager::readFile(Fractal3D::path3DBase);

	std::vector<ShaderSection> sections{};

	Section extraSects = Section("extraSections");
	size_t extraSectionIndex = source.find(extraSects.start);
	if (extraSectionIndex != std::string::npos)
	{
		size_t extraSectionEnd = source.find(extraSects.end);
		std::vector<std::string> sectionContents = splitNotInChar(source.substr(extraSectionIndex + extraSects.start.length(), extraSectionEnd - (extraSectionIndex + extraSects.start.length())), ',', '[', ']');
		for (size_t i = 0; i < sectionContents.size(); i++)
		{
			cleanString(sectionContents[i], {'\n', '\t', ' ', '[', ']', '\"'});
			std::vector<std::string> value = split(sectionContents[i], ',');
			if (value.size() == 1)	sections.push_back(ShaderSection(value[0]));
			else if (value.size() == 2) sections.push_back(ShaderSection(value[0], StringToBool(value[1])));
			else if (value.size() == 3) sections.push_back(ShaderSection(value[0], StringToBool(value[1]), value[2]));
			else if (value.size() == 4) sections.push_back(ShaderSection(value[0], StringToBool(value[1]), value[2], StringToBool(value[3])));
		}
	}

	Fractal3D::ParseShader(source, base, specification, highQuality, specIndex, sections);

	const static std::string vertexSource = FileManager::readFile(Fractal::pathRectangleVertexshader);
	return *(new Shader(vertexSource, base, false));
}
