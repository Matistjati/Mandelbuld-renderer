#include "headers/Fractal2D.h"
#include "headers/FileManager.h"
#include "headers/Debug.h"
#include "headers/Image.h"
#include <algorithm>
#include <nanogui/nanogui.h>
#include <iostream>
#include <fstream>

const std::string& Fractal2D::default2DSource = FileManager::ReadFile(default2DFractal);

#define PrintSource 1

Fractal2D::Fractal2D(int specIndex, int fractalIndex, int fractalNameIndex, glm::vec2 screenSize)
	: Fractal(GenerateShader(specIndex, fractalIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()), fractalNameIndex)),
	screenSize, Time(), GetDefaultShaderIndices(), 1.f, FractalType::fractal2D, fractalIndex, specIndex, fractalNameIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()), fractalNameIndex)),
	power(2), position({ 0,0 })
{	}

Fractal2D::Fractal2D(int specIndex, int fractalIndex, int fractalNameIndex)
	: Fractal(GenerateShader(specIndex, fractalIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()), fractalNameIndex)),
	GetMonitorSize(), Time(), GetDefaultShaderIndices(), 1.f, FractalType::fractal2D, fractalIndex, specIndex, fractalNameIndex, GetFractalNames(FileManager::GetDirectoryFileNames(GetFractalFolderPath()), fractalNameIndex)),
	power(2), position({ 0,0 })
{	}

void Fractal2D::PopulateGUI() 
{
	Fractal::PopulateGUI();


	// Power
 	nanogui::Slider* slider = gui->form->AddSlider("Power",power.value);

	slider->setCallback([this](float value)
		{
			power.SetValue(value, Fractal::renderMode);
			this->shader->SetUniform(this->power);
		});
	slider->setValue(power.value);
	slider->setRange({ 1.f,10.f });
	
	power.guiElements = { slider };
	power.SetGuiValue = [this]() { ((nanogui::Slider*)this->power.guiElements[0])->setValue(this->power.GetValue()); };
	power.SetShaderValue = [this](bool renderMode) {this->shader->SetUniform(this->power, renderMode); };

	// Position
	gui->form->addGroup("Position");
	
	auto positionFieldX = gui->form->AddTextBox("X", position.value.x);
	positionFieldX->setCallback([this](float value)
		{
			position.SetValue({ position.value.x, value }, Fractal::renderMode);
			this->shader->SetUniform(this->position);
		});

	positionFieldX->numberFormat("%.6g");

	auto positionFieldY = gui->form->AddTextBox("Y", position.value.y);
	positionFieldY->setCallback([this](float value)
		{
			position.SetValue({ value, position.value.y }, Fractal::renderMode);
			this->shader->SetUniform(this->position);
		});

	positionFieldY->numberFormat("%.6g");

	position.guiElements = { positionFieldX, positionFieldY };
	position.SetGuiValue = [this]() { 
									  ((nanogui::detail::FormWidget<float, std::true_type>*)this->position.guiElements[0])->setValue(this->position.GetValue().x);
									  ((nanogui::detail::FormWidget<float, std::true_type>*)this->position.guiElements[1])->setValue(this->position.GetValue().y);
									};
	position.SetShaderValue = [this](bool renderMode) {this->shader->SetUniform(this->position, renderMode); };

	Fractal::PopulateGuiFromShader();

	gui->performLayout();
}

void Fractal2D::Update()
{
	Fractal::Update();

	if (shader->type == ShaderType::compute)
	{
		((ComputeShader*)shader)->UseRender();
		glUniform1f(((ComputeShader*)shader)->uniformRenderIds[time.name], (float)time.value.GetTotalTime());
		glUniform1ui(((ComputeShader*)shader)->uniformRenderIds[frame.name], frame.value);
		glUniform1f(((ComputeShader*)shader)->uniformRenderIds[deltaTime.name], deltaTime.value);
		shader->Use();
	}

	if (holdingMouse)
	{
		shader->Use();
		glm::vec2 mouse = (2.f * glm::vec2(mousePosition.value.x, screenSize.value.y - mousePosition.value.y) - (glm::vec2)screenSize.value) / (float)screenSize.value.y * zoom.value;
		clickPositions.value = glm::vec4(mouse, position.value);
		shader->SetUniform(clickPositions);
	}
}

void Fractal2D::MouseCallback(GLFWwindow* window, double x, double y)
{
	mousePosition.value = { x, y };
	shader->SetUniform(mousePosition);
}

void Fractal2D::MousePressCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		holdingMouse = true;
		// Map from screen space to fractal space
		glm::vec2 mouse = (2.f * glm::vec2(mousePosition.value.x, screenSize.value.y - mousePosition.value.y) - (glm::vec2)screenSize.value) / (float)screenSize.value.y * zoom.value;
		clickPositions.value = glm::vec4(mouse, position.value);
		shader->SetUniform(clickPositions);
	}
	else if (action == GLFW_RELEASE)
	{
		holdingMouse = false;
	}
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

		case GLFW_KEY_Q:
			(*shaderIndices["loopReturn"])++;
			update = true;
			break;
		case GLFW_KEY_A:
			(*shaderIndices["loopReturn"])--;
			update = true;
			break;

		case GLFW_KEY_W:
			(*shaderIndices["loopExtraOperations"])++;
			update = true;
			break;
		case GLFW_KEY_S:
			(*shaderIndices["loopExtraOperations"])--;
			update = true;
			break;

		case GLFW_KEY_E:
			(*shaderIndices["loopSetup"])++;
			update = true;
			break;
		case GLFW_KEY_D:
			(*shaderIndices["loopSetup"])--;
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
			case GLFW_KEY_X:
				time.value.ToogleTimePause();
				shader->SetUniform(time);
				break;

		}
	}
}

void Fractal2D::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	screenSize.value = glm::ivec2(width, height);
	shader->SetUniform(screenSize);

	glViewport(0, 0, width, height);
}

void Fractal2D::SetUniforms(Shader* shader, bool computeRender)
{
	if (computeRender) { ((ComputeShader*)shader)->UseRender(); }
	else { shader->Use(); }

	shader->SetUniform(position);
	shader->SetUniform(screenSize);
	shader->SetUniform(power);
	shader->SetUniform(zoom);
	shader->SetUniform(mousePosition);
	shader->SetUniform(time);
	shader->SetUniform(frame);
	shader->SetUniform(clickPositions);
	GlErrorCheck();
}

void Fractal2D::SetUniformLocations(Shader* shader, bool computeRender)
{
	unsigned int id = (computeRender) ? ((ComputeShader*)shader)->renderId : shader->id;
	if (computeRender) { ((ComputeShader*)shader)->UseRender(); }
	else { shader->Use(); }	position.id = glGetUniformLocation(shader->id, position.name.c_str());

	screenSize.id = glGetUniformLocation(id, screenSize.name.c_str());
	power.id = glGetUniformLocation(id, power.name.c_str());
	frame.id = glGetUniformLocation(id, frame.name.c_str());
	zoom.id = glGetUniformLocation(id, zoom.name.c_str());
	mousePosition.id = glGetUniformLocation(id, mousePosition.name.c_str());
	time.id = glGetUniformLocation(id, time.name.c_str());
	clickPositions.id = glGetUniformLocation(id, clickPositions.name.c_str());
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
	clickPositions.name = "clickPositions";
}

void Fractal2D::SaveImage(const std::string path)
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
	catch (const std::exception& e)
	{
		DebugPrint("Error saving image: " + *e.what());
		return;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shader->buffers["privateBuffer"].id);
	void* dat = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

	auto myfile = std::fstream("precomputed/buddhaBrotPoints", std::ios::out | std::ios::binary);
	myfile.write((char*)dat, screenSize.value.x * screenSize.value.y*sizeof(glm::vec4));
	myfile.close();

	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void Fractal2D::FindPathAndSaveImage()
{
	const static std::string baseName = "TestImage/image";
	int count = 0;
	// Finding the first unused file with name-pattern imageN.png where n is the number ascending
	while (FileManager::FileExists((baseName + std::to_string(count) + ".png"))) count++;

	SaveImage(baseName + std::to_string(count) + ".png");
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

void Fractal2D::HandleKeyInput()
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
				position.SetValue({ position.value.x, position.value.y + static_cast<float>(time.value.GetDeltaTime() * parameterChangeRate * zoom.value) }, Fractal::renderMode);
				position.SetGuiValue();
				shader->SetUniform(position);
				break;
			case GLFW_KEY_S:
				position.SetValue({ position.value.x, position.value.y - static_cast<float>(time.value.GetDeltaTime() * parameterChangeRate * zoom.value) }, Fractal::renderMode);
				position.SetGuiValue();
				shader->SetUniform(position);
				break;
			case GLFW_KEY_A:
				position.SetValue({ position.value.x - static_cast<float>(time.value.GetDeltaTime() * parameterChangeRate * zoom.value), position.value.y }, Fractal::renderMode);
				position.SetGuiValue();
				shader->SetUniform(position);
				break;
			case GLFW_KEY_D:
				position.SetValue({ position.value.x + static_cast<float>(time.value.GetDeltaTime() * parameterChangeRate * zoom.value), position.value.y }, Fractal::renderMode);
				position.SetGuiValue();
				shader->SetUniform(position);
				break;

				// Zooming using exponential decay
			case GLFW_KEY_Q:
				zoom.SetValue(zoom.value * static_cast<float>(exp(time.value.GetDeltaTime() * -parameterChangeRate)), Fractal::renderMode);
				zoom.SetGuiValue();
				shader->SetUniform(zoom);
				break;
			case GLFW_KEY_E:
				zoom.SetValue(zoom.value / static_cast<float>(exp(time.value.GetDeltaTime() * -parameterChangeRate)), Fractal::renderMode);
				zoom.SetGuiValue();
				shader->SetUniform(zoom);
				break;

				// Changing the power of the fractal
			case GLFW_KEY_C:
				power.SetValue(power.value + (0.5f * parameterChangeRate * static_cast<float>(time.value.GetDeltaTime())), Fractal::renderMode);
				power.SetGuiValue();
				shader->SetUniform(power);
				break;
			case GLFW_KEY_V:
				power.SetValue(power.value - (0.5f * parameterChangeRate * static_cast<float>(time.value.GetDeltaTime())), Fractal::renderMode);
				power.SetGuiValue();
				shader->SetUniform(power);
				break;

			default:
				break;
			}
		}
	}
}

Shader* Fractal2D::GenerateShader(int* specIndex, int* fractalIndex, std::string name)
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
			else if (value.size() == 4) sections.push_back(ShaderSection(value[0], StringToBool(value[1]), StringToBool(value[3])));
		}
	}

	const std::string specification = FileManager::ReadFile(Fractal2D::GetSpecPath(name));


	return CreateShader(source, &specification, fractalIndex, specIndex, sections);
}

Shader* Fractal2D::GenerateShader()
{
	return GenerateShader(&specIndex, &fractalIndex, fractalName);
}

Shader* Fractal2D::GenerateShader(std::string fractalName)
{
	int* specIndex = new int(0);
	int* fractalIndex = new int(0);
	return Fractal2D::GenerateShader(specIndex, fractalIndex, fractalName);
	delete specIndex;
	delete fractalIndex;
}

Shader* Fractal2D::GenerateShader(int spec, int fractal, std::string fractalName)
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

void Fractal2D::ParseShaderDefault(std::map<ShaderSection, bool> sections, std::string& source, std::string & final, std::string specification)
{
	std::string defaultSource = default2DSource;

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
				from = c.start + GetSection(c, defaultSource) + c.end;
				if (!Replace(defaultSource, from, variables[i]))
				{
					DebugPrint("Couldn't replace variable " + sectionName + " from specification");
				}
			}
		}
	}

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

				int* indexPtr = shaderIndices[c.name];
				if (*indexPtr < 0) *indexPtr = versions.size() - 1;
				else if ((size_t) *indexPtr > versions.size() - 1) *indexPtr = 0;

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

void Fractal2D::ParseShader(std::string& source, std::string & final, const std::string* spec, int* specIndex, int* fractalIndex, const std::vector<ShaderSection> extraSections)
{
	std::map<ShaderSection, bool> sections = std::map<ShaderSection, bool>();

	std::string specSection = GetSpecificationByIndex(spec, specIndex, FileManager::ReadFile(presetSpec2D));
	if (specSection == "")
	{
		DebugPrint("Specification error");
		return;
	}

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
		if (uniforms[i].size()>5)
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

	if (includes != "")
	{
		includes.erase(std::remove(includes.begin(), includes.end(), '\n'), includes.end());
		includes.erase(std::remove(includes.begin(), includes.end(), '\t'), includes.end());
		includes.erase(std::remove(includes.begin(), includes.end(), ' '), includes.end());

		std::vector<std::string> includeList = Fractal::Split(includes, ',');

		const static std::string helperFunctions = FileManager::ReadFile(Fractal2D::helperFunctions);

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
		BuildMainLoop(Section("mainLoop"), source, default2DSource, final, specSection, fractalIndex, (shaderIndices.size() == 0) ? GetDefaultShaderIndices() : shaderIndices);
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

			versions = (source.find(s.start) == std::string::npos) ? SplitNotInChar(GetSection(s, default2DSource), ',', '<', '>') : SplitNotInChar(GetSection(s, source), ',', '<', '>');
			std::string index = GetSection(s, specSection);

			sectionString = versions[std::stoi(index)];

			CleanString(sectionString, { '<', '>' });

			while (Replace(final, s.start, sectionString)) {}
		}
		else
		{
			sectionString = (source.find(s.end) != std::string::npos) ? GetSection(s, source) : GetSection(s, default2DSource);
			while (Replace(final, Section(c.name).start, sectionString)) {}
		}
	}
}

void Fractal2D::Init()
{
	frame.value = 0;

	SetFractalNameFromIndex(&fractalNameIndex, GetFractalFolderPath());
	Fractal::fractalType = FractalType::fractal2D;
	SetVariablesFromSpec(&specIndex, GetSpecPath(fractalName), Fractal2D::presetSpec2D);
	SetUniformNames();

	SetUniformLocations(shader);
	SetUniforms(shader);
	shader->Use();
	GlErrorCheck();

	PopulateGUI();

	if (shader->type == ShaderType::compute)
	{
		ComputeShader* compute = reinterpret_cast<ComputeShader*>(shader);
		compute->Invoke(screenSize.value);
		
		RenderComputeShader();

		compute->UseRender();
		unsigned int id = compute->renderId;
		compute->uniformRenderIds[time.name] = glGetUniformLocation(id, time.name.c_str());
		compute->uniformRenderIds[frame.name] = glGetUniformLocation(id, frame.name.c_str());
		compute->uniformRenderIds[deltaTime.name] = glGetUniformLocation(id, deltaTime.name.c_str());
		shader->Use();
	}
}

std::map<std::string, int*> Fractal2D::GetDefaultShaderIndices()
{
	return { {"loopReturn", new int(0)}, {"loopExtraOperations", new int(0)}, {"loopSetup", new int(0)} };
}

void Fractal2D::RenderComputeShader()
{
	((ComputeShader*)shader)->UseRender();

	ComputeShader* explShader = reinterpret_cast<ComputeShader*>(shader);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, explShader->buffers[Fractal::rectangleVertexBufferIndexName].id);
	glBindVertexArray(explShader->buffers[Fractal::rectangleVertexBufferName].id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, explShader->buffers[computeRenderBufferName].id);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, explShader->buffers[computeRenderBufferName].binding, explShader->mainBuffer.id);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	shader->Use();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, explShader->mainBuffer.id);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, explShader->mainBuffer.binding, explShader->mainBuffer.id);
}

void Fractal2D::SetShaderGui(bool render)
{
	power.SetGuiValue();
	position.SetGuiValue();
	Fractal::SetShaderGui(render);
}

void Fractal2D::SetShaderUniforms(bool render)
{
	power.SetShaderValue(render);
	position.SetShaderValue(render);
	Fractal::SetShaderUniforms(render);
}

std::vector<int> GetPrimeFactors(int n)
{
	std::vector<int> factors = std::vector<int>();


	while (n % 2 == 0)
	{
		factors.push_back(2);
		n /= 2;
	}

	int root = int(ceil(sqrt(n)));

	for (int i = 3; i <= root; i+=2)
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

Shader* Fractal2D::CreateShader(std::string source, const std::string* specification, int* fractalIndex, int* specIndex, std::vector<ShaderSection> shaderSections)
{
	const static std::string vertexSource = FileManager::ReadFile(Fractal::pathRectangleVertexshader);

	std::string renderBase;
	std::string type = GetSection(Section("type"), source);
	if (type != "")
	{
		if (type == "fragment")
		{
			renderBase = FileManager::ReadFile(Fractal2D::path2DBase);
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

			std::string computeBase = FileManager::ReadFile(Fractal2D::path2DBaseCompute);

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
			renderSource = FileManager::ReadFile(Fractal2D::fractal2dPath + renderSourceName);

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


			std::string renderBase = FileManager::ReadFile(Fractal2D::path2DBase);

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
		renderBase = FileManager::ReadFile(Fractal2D::path2DBase);
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
