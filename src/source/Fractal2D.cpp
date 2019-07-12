#include "headers/Fractal2D.h"
#include "headers/FileManager.h"
#include "headers/Debug.h"
#include "headers/Image.h"
#include <algorithm>

template <typename T>
T clamp(const T& n, const T& lower, const T& upper)
{
	return std::max(lower, std::min(n, upper));
}

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

void Fractal2D::Update() { }

void Fractal2D::MouseCallback(GLFWwindow* window, double x, double y)
{
	mousePosition = { x, y };
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
	GlErrorCheck();
}

void Fractal2D::SetUniformLocations(Shader* shader)
{
	shader->use();
	position.id = glGetUniformLocation(shader->id, position.name.c_str());
	screenSize.id = glGetUniformLocation(shader->id, screenSize.name.c_str());
	power.id = glGetUniformLocation(shader->id, power.name.c_str());
	zoom.id = glGetUniformLocation(shader->id, zoom.name.c_str());
	GlErrorCheck();
}

void Fractal2D::SetUniformNames()
{
	position.name = "position";
	power.name = "power";
	screenSize.name = "screenSize";
	zoom.name = "zoom";
}

void Fractal2D::SaveImage(const std::string path)
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
}

void Fractal2D::SetVariablesFromSpec(int* index, std::string specification)
{
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
				position.value.y += static_cast<float>(time.value.deltaTime * parameterChangeRate * zoom.value);
				explorationShader->SetUniform(position);
				break;
			case GLFW_KEY_S:
				position.value.y -= static_cast<float>(time.value.deltaTime * parameterChangeRate * zoom.value);
				explorationShader->SetUniform(position);
				break;
			case GLFW_KEY_A:
				position.value.x -= static_cast<float>(time.value.deltaTime * parameterChangeRate * zoom.value);
				explorationShader->SetUniform(position);
				break;
			case GLFW_KEY_D:
				position.value.x += static_cast<float>(time.value.deltaTime * parameterChangeRate * zoom.value);
				explorationShader->SetUniform(position);
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

				// Zooming using exponential decay
			case GLFW_KEY_Q:
				zoom.value *= static_cast<float>(exp(time.value.deltaTime * -parameterChangeRate));
				explorationShader->SetUniform(zoom);
				break;
			case GLFW_KEY_E:
				zoom.value /= static_cast<float>(exp(time.value.deltaTime * -parameterChangeRate));
				explorationShader->SetUniform(zoom);
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

			default:
				break;
			}
		}
	}
}

std::pair<Shader*, Shader*> Fractal2D::GenerateShader(int* specIndex, int* fractalIndex, std::string name)
{
	//GlErrorCheck();

	//std::string base = FileManager::ReadFile(Fractal2D::path2DBase);

	//std::vector<ShaderSection> sections{};

	//std::string source = FileManager::ReadFile(Fractal2D::GetFractalPath(name));

	//Section extraSects = Section("extraSections");
	//size_t extraSectionIndex = source.find(extraSects.start);
	//if (extraSectionIndex != std::string::npos)
	//{
	//	size_t extraSectionEnd = source.find(extraSects.end);
	//	std::vector<std::string> sectionContents = SplitNotInChar(source.substr(extraSectionIndex + extraSects.start.length(), extraSectionEnd - (extraSectionIndex + extraSects.start.length())), ',', '[', ']');
	//	for (size_t i = 0; i < sectionContents.size(); i++)
	//	{
	//		CleanString(sectionContents[i], { '\n', '\t', ' ', '[', ']', '\"' });
	//		std::vector<std::string> value = Split(sectionContents[i], ',');
	//		if (value.size() == 1)	sections.push_back(ShaderSection(value[0]));
	//		else if (value.size() == 2) sections.push_back(ShaderSection(value[0], StringToBool(value[1])));
	//		else if (value.size() == 3) sections.push_back(ShaderSection(value[0], StringToBool(value[1]), value[2]));
	//		else if (value.size() == 4) sections.push_back(ShaderSection(value[0], StringToBool(value[1]), value[2], StringToBool(value[3])));
	//	}
	//}

	//const std::string specification = FileManager::ReadFile(Fractal2D::GetSpecPath(name));

	//std::string sourceCopy = std::string(source);
	//std::string baseCopy = std::string(base);
	//ParseShader(sourceCopy, baseCopy, &specification, false, specIndex, fractalIndex, sections);

	//ParseShader(source, base, &specification, true, specIndex, fractalIndex, sections);

	//const static std::string vertexSource = FileManager::ReadFile(Fractal::pathRectangleVertexshader);

	//return std::pair<Shader*, Shader*>((new Shader(vertexSource, baseCopy, false)),
	//	(new Shader(vertexSource, base, false)));

	const static std::string vertexSource = FileManager::ReadFile(Fractal::pathRectangleVertexshader);

	std::string base = FileManager::ReadFile(Fractal2D::GetFractalPath(name));

	return std::pair<Shader*, Shader*>((new Shader(vertexSource, base, false)),
									   (new Shader(vertexSource, base, false)));
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

void Fractal2D::Init()
{
	Fractal::fractalType = FractalType::fractal2D;
	SetVariablesFromSpec(&specIndex, GetSpecPath(fractalName));
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	explorationShader->use();
	GlErrorCheck();
}

std::map<std::string, int*> Fractal2D::GetDefaultShaderIndices()
{
	return {};
}
