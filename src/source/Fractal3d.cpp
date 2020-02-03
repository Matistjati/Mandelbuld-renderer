#include "headers/FileManager.h"
#include "headers/Fractal3D.h"
#include "headers/Image.h"
#include "headers/Debug.h"
#include <map>
#include <algorithm>
#include <thread>

const std::string& Fractal3D::default3DSource = FileManager::ReadFile(default3DFractal);

#define PrintSource 1

Fractal3D::Fractal3D(float power, Shader* explorationShader, Shader* renderShader, Camera& camera, glm::vec3 sun, glm::vec2 screenSize, Time time, int* specIndex, std::string specification)
	: Fractal(screenSize, time, {}), camera(camera), sun(sun), cursorVisible(false)
{
	shader = explorationShader;
}

Fractal3D::Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex, glm::vec2 screenSize)
	: Fractal(screenSize, Time(), 1.f, FractalType::fractal3D, fractalIndex, specIndex,
		fractalNameIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()), fractalNameIndex)),
	camera(DefaultCamera),
	sun(glm::normalize(glm::vec3(0.577, 0.577, 0.577))),
	cursorVisible(false)
{
	shader = GenerateShader(specIndex, fractalIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()), fractalNameIndex));
}

Fractal3D::Fractal3D(int specIndex, int fractalIndex, int fractalNameIndex)
	: Fractal(GetMonitorSize(), Time(), 1.f, FractalType::fractal3D, fractalIndex, specIndex,
		fractalNameIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()), fractalNameIndex)),
	camera(DefaultCamera), 
	sun(glm::normalize(glm::vec3(0.577, 0.577, 0.577))),
	cursorVisible(false)
{
	shader = GenerateShader(specIndex, fractalIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()), fractalNameIndex));
}

void Fractal3D::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Fractal::KeyCallback(window, key, scancode, action, mods);

	if (action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL) != GLFW_MOD_CONTROL)
	{
		switch (key)
		{
		default:
			break;
		case GLFW_KEY_F:
			cursorVisible = !cursorVisible;
			if (cursorVisible)
			{
				double x;
				double y;
				glfwGetCursorPos(window, &x, &y);
				lastNonGuiPos = { x,y };
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwSetCursorPos(window, lastNonGuiPos.x, lastNonGuiPos.y);
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

	shader->SetUniform(camera.GetRotationMatrix());
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

void Fractal3D::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	zoom.SetValue(zoom.GetValue() * static_cast<float>(yoffset * time.value.GetDeltaTime() * scrollSpeed + 1), Fractal::renderMode);
	zoom.SetValue(glm::max(1.0f, zoom.GetValue()), Fractal::renderMode);
	zoom.SetGuiValue();
	shader->SetUniform(zoom);
}

void Fractal3D::SetUniforms(Shader* shader, bool computeRender)
{
	Fractal::SetUniforms(shader, computeRender);

	shader->SetUniform(camera.position);
	shader->SetUniform(camera.GetRotationMatrix());
	shader->SetUniform(sun);
	GlErrorCheck();
}

void Fractal3D::SetUniformLocations(Shader* shader, bool computeRender)
{
	Fractal::SetUniformLocations(shader, computeRender);
	
	camera.GetRotationMatrix().id = glGetUniformLocation(shader->id, camera.GetRotationMatrix().name.c_str());
	camera.position.id =			glGetUniformLocation(shader->id, camera.position.name.c_str());
	sun.id =						glGetUniformLocation(shader->id, sun.name.c_str());
	GlErrorCheck();
}

void Fractal3D::SetUniformNames()
{
	Fractal::SetUniformNames();

	camera.GetRotationMatrix().name = "rotation";
	camera.position.name = "position";
	sun.name = "sun";
	GlErrorCheck();
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
	shader->SetUniform(sun);
}

void Fractal3D::ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string& final, std::string specification)
{
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

		while(Replace(final, s.start, sectionString)) {}
	}

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
					if (value[value.size() - 1 ] == ']') value = value.substr(0, value.size()-1);

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
}

void Fractal3D::ParseShader(std::string& source, std::string& final, const std::string* spec, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections)
{
	std::map<ShaderSection, bool> sections = std::map<ShaderSection, bool>();

	std::string specSection = GetSpecificationByIndex(spec, specIndex, FileManager::ReadFile(presetSpec3D));
	if (specSection == "")
	{
		DebugPrint("Specification error");
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

	BuildMainLoop(Section("distanceEstimator"),	source, default3DSource, final, specSection, fractalIndex, shaderIndices);

	std::string flags = GetSection(Section("flags"), specSection);

	const static size_t sectionSize = std::extent<decltype(shaderSections)>::value;
	for (size_t i = 0; i < sectionSize; i++)
	{
		ShaderSection c = shaderSections[i];
		sections[c] = false;

		Section s = Section(c.name);

		if (flags.find("<" + GetSectionName(s.start) + "Default>") == std::string::npos)
		{
			sections[c] = ReplaceSection(s, Section(c.name), source, final);
		}
	}




	ParseShaderDefault(sections, source, final, specSection);

	for (size_t i = 0; i < extraSections.size(); i++)
	{
		ShaderSection c = extraSections[i];
		
		Section s = Section(c.name);

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
	SetVariablesFromSpec(&specIndex, GetSpecPath(fractalName), Fractal3D::presetSpec3D);
	SetUniformNames();

	SetUniformLocations(shader);
	SetUniforms(shader);
	shader->Use();
	GlErrorCheck();

	PopulateGUI();

	Fractal::Init();
}

void Fractal3D::SetShaderGui(bool render)
{
	Fractal::SetShaderGui(render);
}

void Fractal3D::SetShaderUniforms(bool render)
{
	Fractal::SetShaderUniforms(render);
}

// This is really nasty, be careful
inline void Fractal3D::SetVariable(std::string name, std::string value)
{
	if (name == "position")
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
				camera.ProcessMovement(CameraMovement::forward, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate / zoom.value);
				shader->SetUniform(camera.position);
				break;
			case GLFW_KEY_S:
				camera.ProcessMovement(CameraMovement::back, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate / zoom.value);
				shader->SetUniform(camera.position);
				break;
			case GLFW_KEY_A:
				camera.ProcessMovement(CameraMovement::left, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate / zoom.value);
				shader->SetUniform(camera.position);
				break;
			case GLFW_KEY_D:
				camera.ProcessMovement(CameraMovement::right, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate / zoom.value);
				shader->SetUniform(camera.position);
				break;

				// Up and down
			case GLFW_KEY_SPACE:
				camera.ProcessMovement(CameraMovement::up, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate / zoom.value);
				shader->SetUniform(camera.position);
				break;
			case GLFW_KEY_LEFT_SHIFT:
				camera.ProcessMovement(CameraMovement::down, static_cast<float>(time.value.GetDeltaTime()) * parameterChangeRate / zoom.value);
				shader->SetUniform(camera.position);
				break;

				// Camera roll
			case GLFW_KEY_Q:
				camera.ProcessRoll(static_cast<float>(camera.rollSpeed * time.value.GetDeltaTime() * parameterChangeRate / zoom.value));
				shader->SetUniform(camera.GetRotationMatrix());
				break;
			case GLFW_KEY_E:
				camera.ProcessRoll(-static_cast<float>(camera.rollSpeed * time.value.GetDeltaTime() * parameterChangeRate / zoom.value));
				shader->SetUniform(camera.GetRotationMatrix());
				break;

			default:
				break;
			}
		}
	}
}

Shader* Fractal3D::GenerateShader(int* specIndex, int* fractalIndex, std::string name)
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
			else if (value.size() == 4) sections.push_back(ShaderSection(value[0], StringToBool(value[1]), StringToBool(value[3])));
		}
	}

	const std::string specification = FileManager::ReadFile(Fractal3D::GetSpecPath(name));

	ParseShader(source, base, &specification, specIndex, fractalIndex, sections);

	const static std::string vertexSource = FileManager::ReadFile(Fractal::pathRectangleVertexshader);


	fractalSourceCode = base;

#if PrintSource
	std::cout << base;
#endif

	return new Shader(vertexSource, base);
}

Shader* Fractal3D::GenerateShader()
{
	return GenerateShader(&specIndex, &fractalIndex, fractalName);
}

Shader* Fractal3D::GenerateShader(std::string fractalName)
{
	int* specIndex = new int(0);
	int* fractalIndex = new int(0);
	Shader* shader = Fractal3D::GenerateShader(specIndex, fractalIndex, fractalName);
	delete specIndex;
	delete fractalIndex;
	return shader;
}

Shader* Fractal3D::GenerateShader(int spec, int fractal, std::string fractalName)
{
	int* specIndex = new int(spec);
	int* fractalIndex = new int(fractal);
	Shader* shader = Fractal3D::GenerateShader(specIndex, fractalIndex, fractalName);
	delete specIndex;
	delete fractalIndex;
	return shader;
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
