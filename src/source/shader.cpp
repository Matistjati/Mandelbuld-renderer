#include "headers/Shader.h"
#include <glad/glad.h>

#include "headers/Debug.h"
#include "headers/FileManager.h"
#include "headers/Fractal.h"
#include "headers/BufferInitialization.h"
#include "headers/Image.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream> 

void Shader::Use()
{
	glUseProgram(id);
}

Shader::~Shader()
{
	glDeleteProgram(id);
	GlErrorCheck();

	for (auto& buffer : buffers)
	{
		buffer.second.Delete();
	}
	for (auto& texture : textures)
	{
		texture.Delete();
	}
	GlErrorCheck();
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, bool path) : lowestAvailableTextureUnit(0)
{
	type = ShaderType::fragment;
	std::string vShaderCode = path ? FileManager::ReadFile(vertexPath).c_str() : vertexPath.c_str();
	std::string fShaderCode = path ? FileManager::ReadFile(fragmentPath).c_str() : fragmentPath.c_str();

	id = CreateFragmentProgram(vShaderCode, fShaderCode);


	GlErrorCheck();
}

Shader::Shader(unsigned int id, ShaderType type) : id(id), type(type), lowestAvailableTextureUnit(0) {}

unsigned int Shader::CreateFragmentProgram(const std::string& vertex, const std::string& fragment)
{
	unsigned int id;
	id = glCreateProgram();

	unsigned int vertexId = CompileShader(GL_VERTEX_SHADER, vertex);
	unsigned int fragmentId = CompileShader(GL_FRAGMENT_SHADER, fragment);

	glAttachShader(id, vertexId);
	glAttachShader(id, fragmentId);
	glLinkProgram(id);


	int success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		int length;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> infoLog(length);
		glGetProgramInfoLog(id, length, NULL, &infoLog[0]);
		std::cerr << "Error: program linking failed\n" << std::string(infoLog.begin(), infoLog.end()) << std::endl;
	}


	glValidateProgram(id);

	const float vertices[12] =
	{
		// Positions
		 1.f,  1.f, 0.0f, // top right
		 1.f, -1.f, 0.0f, // bottom right
		-1.f,  1.f, 0.0f, // top left
		-1.f, -1.f, 0.0f, // bottom left
	};

	const unsigned int indices[6] =
	{
		0, 1, 2,
		1, 2, 3
	};

	unsigned int vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	unsigned int vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(vertexArray);

	unsigned int vertexIndices;
	glGenBuffers(1, &vertexIndices);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndices);
	glBindVertexArray(vertexArray);



	buffers[Fractal::rectangleVertexBufferName] = Buffer(vertexArray, Buffer::BufferType::vertexArray);
	buffers[Fractal::rectangleVertexBufferIndexName] = Buffer(vertexIndices);

	if (fragment.find("<bufferType>") != std::string::npos)
	{
		std::vector<Buffer> buffer = GenerateBuffersForProgram(fragment);

		for (size_t i = 0; i < buffer.size(); i++)
		{
			if (buffer[i].name.find("private") != std::string::npos)
			{
				continue;
			}
			else
			{
				buffers[buffer[i].name] = buffer[i];
			}
		}
	}

	if (fragment.find("<TextureInfo>") != std::string::npos)
	{
		std::vector<Texture> texture = GenerateTexturesForProgram(fragment, id);

		textures = texture;
	}

	glDeleteShader(vertexId);
	glDeleteShader(fragmentId);

	return id;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	static std::map<int, std::string> shaderTypeToString = { {GL_VERTEX_SHADER, "vertex"}, {GL_FRAGMENT_SHADER, "fragment"}, {GL_COMPUTE_SHADER, "compute"} };

	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		DebugPrint(source);
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> infoLog(length);
		glGetShaderInfoLog(id, length, NULL, &infoLog[0]);

		std::string errorString =  std::string(infoLog.begin(), infoLog.end());
		std::vector<std::string> lines = Fractal::Split(source, '\n');
		std::vector<std::string> errorLineNumbers = Fractal::GetCharsWithinDelimeters(errorString, "0(", ")");
		std::vector<std::string> errorStrings;

		for (size_t i = 0; i < errorLineNumbers.size(); i++)
		{
			try
			{
				errorStrings.push_back(lines[std::stoi(errorLineNumbers[i])-1]);
			}
			catch (const std::exception&)
			{
				std::cout << "Invalid line number displaying error.";
			}
		}

		std::cerr << "Failed to compile " << shaderTypeToString[type] << " shader.\nError: " << errorString << std::endl;
		for (size_t i = 0; i < errorLineNumbers.size(); i++)
		{
			if (i < errorString.size())
			{
				std::cout << errorLineNumbers[i] << ": " << Fractal::Trim(errorStrings[i]) << std::endl;
			}
		}
		glDeleteShader(id);
		return -1;
	}

	return id;
}

uint32_t randomInt(uint32_t upperBound)
{
	static uint32_t x = 123456789;
	static uint32_t y = 362436069;
	static uint32_t z = 521288629;
	static uint32_t w = 88675123;
	uint32_t t;
	t = x ^ (x << 11);
	x = y; y = z; z = w;
	w = w ^ (w >> 19) ^ (t ^ (t >> 8));
	return uint32_t(float(w) / float(UINT32_MAX) * upperBound);
}

std::vector<Buffer> Shader::GenerateBuffersForProgram(std::string source)
{
	std::vector<Buffer> buffers(0);
	size_t offset = 0;
	while (true)
	{
		size_t bufferTypeStart = source.find("<bufferType>", offset);

		if (bufferTypeStart != std::string::npos)
		{
			bufferTypeStart += (std::string("<bufferType>")).length();
			std::string type = source.substr(bufferTypeStart, source.find("</bufferType>", bufferTypeStart) - bufferTypeStart);

			int binding = -1;

			size_t bindingStart = source.find("binding", bufferTypeStart);

			if (bindingStart != std::string::npos)
			{
				bindingStart += std::string("binding").length();
				std::string bindingStr = source.substr(bindingStart, source.find(')', bindingStart) - bindingStart);
				static const std::vector<char> blackList = { '=',' ' };

				for (size_t i = 0; i < blackList.size(); i++)
				{
					bindingStr.erase(std::remove(bindingStr.begin(), bindingStr.end(), blackList[i]), bindingStr.end());
				}

				binding = std::stoi(bindingStr);
			}
			else
			{
				DebugPrint("Fatal error: could not find binding for mainbuffer at compute shader");
				binding = 0;
			}

			GLuint buffer;
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);

			bool initialized = false;
			size_t end = source.find(";", bufferTypeStart);
			size_t bufferInitStart = source.find("<cpuInitialize>", offset);
			if (bufferInitStart != std::string::npos && bufferInitStart < end)
			{
				bufferInitStart += (std::string("<cpuInitialize>")).length();
				std::string type = source.substr(bufferInitStart, source.find("</cpuInitialize>") - bufferInitStart);

				std::vector<glm::vec4> data(int(Fractal::screenSize.value.x * Fractal::screenSize.value.y));
				if (type.find("(") != std::string::npos)
				{
					std::string functionName = type.substr(0, type.find("("));
					if (BufferInitialization::functions.count(functionName))
					{
						std::vector<float> params;
						size_t paramStart = type.find("(");
						if (paramStart != std::string::npos)
						{
							std::string parameters = type.substr(paramStart + 1, type.length() - paramStart);
							std::vector<std::string> values;
							std::stringstream test(parameters);
							std::string segment;

							while (std::getline(test, segment, ','))
							{
								values.push_back(segment);
							}

							for (size_t i = 0; i < values.size(); i++)
							{
								params.push_back(std::stof(values[i]));
							}
						}

						BufferInitialization::functions[functionName](data, Fractal::screenSize.value, params);
						glBufferData(GL_SHADER_STORAGE_BUFFER, int(Fractal::screenSize.value.x * Fractal::screenSize.value.y * sizeof(glm::vec4)), &data[0], GL_DYNAMIC_DRAW);
						initialized = true;
					}
				}
				else
				{
					auto myfile = std::ifstream("precomputed/buddhaBrotPoints", std::ios::ate | std::ios::binary);

					std::streamsize size = myfile.tellg();
					myfile.seekg(0, std::ios::beg);

					if (!myfile.read((char*)&data[0], std::min(data.size()*sizeof(glm::vec4), (unsigned int)size)))
					{
						DebugPrint("Critical error: could not read the precomputed file");
						BreakIfDebug();
					}
					myfile.close();
					glBufferData(GL_SHADER_STORAGE_BUFFER, int(Fractal::screenSize.value.x * Fractal::screenSize.value.y * sizeof(glm::vec4)), &data[0], GL_DYNAMIC_DRAW);
					initialized = true;
				}

			}
			if (!initialized)
			{
				glBufferData(GL_SHADER_STORAGE_BUFFER, int(Fractal::screenSize.value.x * Fractal::screenSize.value.y * sizeof(glm::vec4)), nullptr, GL_DYNAMIC_DRAW);
			}

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);

			offset = bufferTypeStart;
			GlErrorCheck();
			Buffer newBuffer = Buffer(buffer, binding, type);
			if (!initialized)
			{
				newBuffer.Clear();
			}
			buffers.push_back(newBuffer);
		}
		else
		{
			break;
		}
	}
	return buffers;
}

std::vector<Texture> Shader::GenerateTexturesForProgram(std::string source, unsigned int id)
{
	glUseProgram(id);
	const std::string start = "<TextureInfo>";
	const std::string end = "</TextureInfo>";
	std::vector<Texture> textures(0);
	size_t offset = 0;
	while (true)
	{
		size_t textureInfoStart = source.find(start, offset);
		size_t textureInfoEnd = source.find(end, textureInfoStart);

		std::string fileName = "";
		std::string textureName = "";

		if (textureInfoStart != std::string::npos)
		{
			std::string textureContent = Fractal::SubString(source, textureInfoStart+start.size(), textureInfoEnd);

			std::vector<std::string> content = Fractal::SplitNotInChar(textureContent, ',', { {'[',']'}, {'{','}'} });
			
			for (size_t i = 0; i < content.size(); i++)
			{
				std::vector<std::string> entry = Fractal::SplitNotInChar(content[i], ':', { {'[',']'}, {'{','}'} });
				std::string key = entry[0];
				Fractal::CleanString(key, { ' ', '	', '\n' });
				if (key == "FileName")
				{
					std::string value = entry[1];
					size_t quoteStart = value.find("\"");
					size_t quoteEnd = value.rfind("\"");
					fileName = Fractal::SubString(value, quoteStart+1, quoteEnd);
				}
			}

			size_t uniformStart = source.find("uniform", textureInfoStart);
			if (uniformStart != std::string::npos)
			{
				size_t delimeterLocation = source.find(";", uniformStart+8);
				size_t nameStart = std::string::npos;
				size_t nameEnd = std::string::npos;
				for (int i = delimeterLocation; i >= 0; i--)
				{
					if (source[i] != ' ' && source[i] != '	' && source[i] != '\n')
					{
						nameEnd = i;
						break;
					}
				}
				nameStart = source.rfind(" ", nameEnd);

				textureName = Fractal::SubString(source, nameStart+1, nameEnd);
			}
			else
			{
				DebugPrint("Could not find texture sampler uniform");
				BreakIfDebug();
			}

			if (fileName != "")
			{
				std::string filePath = "";
				if (FileManager::FileExists(fileName)) filePath = fileName;
				else if (FileManager::FileExists("textures/"+fileName)) filePath = "textures/" + fileName;
				else
				{
					DebugPrint("Could not find the file location of specified texture " + filePath);
					BreakIfDebug();
				}

				Image textureImage = Image(filePath);

				unsigned int textureId;
				glGenTextures(1, &textureId);

				Texture texture = Texture(textureId, lowestAvailableTextureUnit);
				lowestAvailableTextureUnit += 1;

				unsigned int textureLocation = glGetUniformLocation(id, textureName.c_str());
				if (textureLocation == unsigned int (-1))
				{
					DebugPrint("Could not find uniform texture " + textureName);
					//BreakIfDebug();
				}
				glUniform1i(textureLocation, texture.unitId);
				glActiveTexture(GL_TEXTURE0 + texture.unitId);
				glBindTexture(GL_TEXTURE_2D, textureId);


				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width, textureImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(&textureImage.pixels[0])[0][0]);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


				
				glActiveTexture(GL_TEXTURE0 + texture.unitId);
				glBindTexture(GL_TEXTURE_2D, textureId);
				textures.push_back(texture);
			}

			offset = textureInfoEnd + end.size();
		}
		else
		{
			break;
		}

	}


	return textures;
}

void UseProgramIfValid(unsigned int id)
{
	if (id != 0xFFFFFFFF)
	{
		glUseProgram(id);
	}
}

void Shader::SetUniform(const Uniform<float> value) const
{
	UseProgramIfValid(value.programId);
	glUniform1f(value.id, value.value);
}

void Shader::SetUniform(Uniform<float> value, bool renderMode) const
{
	UseProgramIfValid(value.programId);
	glUniform1f(value.id, (renderMode) ? value.renderValue : value.value);
}

void Shader::SetUniform(Uniform<int> value) const
{
	UseProgramIfValid(value.programId);
	glUniform1i(value.id, value.value);
}

void Shader::SetUniform(Uniform<int> value, bool renderMode) const
{
	UseProgramIfValid(value.programId);
	glUniform1i(value.id, (renderMode) ? value.renderValue : value.value);
}

void Shader::SetUniform(Uniform<unsigned int> value) const
{
	UseProgramIfValid(value.programId);
	glUniform1ui(value.id, value.value);
}

void Shader::SetUniform(Uniform<bool> value) const
{
	UseProgramIfValid(value.programId);
	glUniform1i(value.id, value.value);
}

void Shader::SetUniform(Uniform<bool> value, bool renderMode) const
{
	UseProgramIfValid(value.programId);
	glUniform1i(value.id, (renderMode) ? value.renderValue : value.value);
}

void Shader::SetUniform(Uniform<glm::ivec2> vector) const
{
	UseProgramIfValid(vector.programId);
	glUniform2i(vector.id, vector.value.x, vector.value.y);
}

void Shader::SetUniform(Uniform<glm::vec2> vector, bool renderMode) const
{
	UseProgramIfValid(vector.programId);
	glm::vec2 v = (renderMode) ? vector.renderValue : vector.value;
	glUniform2f(vector.id, v.x, v.y);
}

void Shader::SetUniform(Uniform<glm::vec2> vector) const
{
	UseProgramIfValid(vector.programId);
	glUniform2f(vector.id, vector.value.x, vector.value.y);
}

void Shader::SetUniform(Uniform<glm::vec3> vector) const
{
	UseProgramIfValid(vector.programId);
	glUniform3f(vector.id, vector.value.x, vector.value.y, vector.value.z);
}

void Shader::SetUniform(Uniform<glm::vec3> vector, bool renderMode) const
{
	UseProgramIfValid(vector.programId);
	glm::vec3 v = (renderMode) ? vector.renderValue : vector.value;
	glUniform3f(vector.id, v.x, v.y, v.z);
}

void Shader::SetUniform(Uniform<Time> value) const
{
	UseProgramIfValid(value.programId);
	glUniform1f(value.id, static_cast<float>(value.value.GetTotalTime()));
}

void Shader::SetUniform(Uniform<glm::vec4> vector) const
{
	UseProgramIfValid(vector.programId);
	glUniform4f(vector.id, vector.value.x, vector.value.y, vector.value.z, vector.value.w);
}

void Shader::SetUniform(Uniform<glm::vec4> vector, bool renderMode) const
{
	UseProgramIfValid(vector.programId);
	glm::vec4 v = (renderMode) ? vector.renderValue : vector.value;
	glUniform4f(vector.id, v.x, v.y, v.z, v.w);
}

void Shader::SetUniform(Uniform<glm::mat2>& mat) const
{
	UseProgramIfValid(mat.programId);
	glUniformMatrix2fv(mat.id, 1, GL_FALSE, &mat.value[0][0]);
}

void Shader::SetUniform(Uniform<glm::mat3>& mat) const
{
	UseProgramIfValid(mat.programId);
	glUniformMatrix3fv(mat.id, 1, GL_FALSE, &mat.value[0][0]);
}

void Shader::SetUniform(Uniform<glm::mat4>& mat) const
{
	UseProgramIfValid(mat.programId);
	glUniformMatrix4fv(mat.id, 1, GL_FALSE, &mat.value[0][0]);
}

void Shader::SetUniform(Uniform<nanogui::Color>& color) const
{
	UseProgramIfValid(color.programId);
	glUniform3f(color.id, color.value.x(), color.value.y(), color.value.z());
}

void Shader::SetUniform(Uniform<nanogui::Color>& color, bool renderMode) const
{
	UseProgramIfValid(color.programId);
	nanogui::Color c = (renderMode) ? color.renderValue : color.value;
	glUniform3f(color.id, c.x(), c.y(), c.z());
}

void Shader::SetUniform(unsigned int id, float x, float y, float z) const
{
	glUniform3f(id, x, y, z);
}

void Shader::SetUniform(unsigned int id, float value) const
{
	glUniform1f(id, value);
}

void Shader::SetUniform(unsigned int id, int value) const
{
	glUniform1i(id, value);
}

void Shader::SetUniform(unsigned int id, unsigned int value) const
{
	glUniform1ui(id, value);
}

void Shader::SetUniformStr(Uniform<glm::vec2> vector) const
{
	UseProgramIfValid(vector.programId);
	glUniform2f(glGetUniformLocation(id, vector.name.c_str()), vector.value.x, vector.value.y);
}

void Shader::SetUniformStr(Uniform<glm::vec3> vector) const
{
	UseProgramIfValid(vector.programId);
	glUniform3f(glGetUniformLocation(id, vector.name.c_str()), vector.value.x, vector.value.y, vector.value.z);
}

void Shader::SetUniformStr(Uniform<glm::vec4> vector) const
{
	UseProgramIfValid(vector.programId);
	glUniform4f(glGetUniformLocation(id, vector.name.c_str()), vector.value.x, vector.value.y, vector.value.z, vector.value.w);
}

void Shader::SetUniformStr(Uniform<glm::mat2>& mat) const
{
	UseProgramIfValid(mat.programId);
	glUniformMatrix2fv(glGetUniformLocation(id, mat.name.c_str()), 1, GL_FALSE, &mat.value[0][0]);
}

void Shader::SetUniformStr(Uniform<glm::mat3>& mat) const
{
	UseProgramIfValid(mat.programId);
	glUniformMatrix3fv(glGetUniformLocation(id, mat.name.c_str()), 1, GL_FALSE, &mat.value[0][0]);
}

void Shader::SetUniformStr(Uniform<glm::mat4>& mat) const
{
	UseProgramIfValid(mat.programId);
	glUniformMatrix4fv(glGetUniformLocation(id, mat.name.c_str()), 1, GL_FALSE, &mat.value[0][0]);
}

void Shader::SetUniformStr(Uniform<bool> value) const
{
	UseProgramIfValid(value.programId);
	glUniform1i(glGetUniformLocation(id, value.name.c_str()), value.value);
}

void Shader::SetUniformStr(Uniform<int> value) const
{
	UseProgramIfValid(value.programId);
	glUniform1i(glGetUniformLocation(id, value.name.c_str()), value.value);
}

void Shader::SetUniformStr(Uniform<unsigned int> value) const
{
	UseProgramIfValid(value.programId);
	glUniform1ui(glGetUniformLocation(id, value.name.c_str()), value.value);
}

void Shader::SetUniformStr(Uniform<float> value) const
{
	UseProgramIfValid(value.programId);
	glUniform1f(glGetUniformLocation(id, value.name.c_str()), value.value);
}

void Shader::SetUniformStr(const std::string& name, int x) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), x);
}

void Shader::SetUniformStr(const std::string& name, float x) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), x);
}

void Shader::SetUniformStr(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void Shader::SetUniformStr(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void Shader::SetUniformStr(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void Shader::SetUniformStr(const std::string& name, glm::vec3 vector) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), vector.x, vector.y, vector.z);
}

void ComputeShader::UseRender()
{
	glUseProgram(renderId);
}

ComputeShader::ComputeShader(const std::string& computePath, std::string vertexPath, std::string renderPath, bool path, glm::ivec3 groupSize, int renderingFrequency)
	: Shader(CreateProgram(path ? FileManager::ReadFile(computePath) : computePath), ShaderType::compute),
	renderId(CreateFragmentProgram(path ? FileManager::ReadFile(vertexPath) : vertexPath, path ? FileManager::ReadFile(renderPath) : renderPath)),
	groupSize(groupSize), renderingFrequency(renderingFrequency), uniformRenderIds()
{
	glUseProgram(id);
	std::string source = path ? FileManager::ReadFile(computePath) : computePath;
	std::vector<Buffer> buffer = GenerateBuffersForProgram(source);

	for (size_t i = 0; i < buffer.size(); i++)
	{
		if (buffer[i].name == "mainBuffer")
		{
			mainBuffer = buffer[i];
		}
		else
		{
			buffers[buffer[i].name] = buffer[i];
		}
	}
}

void ComputeShader::Invoke(glm::ivec2 screenSize)
{
	this->Use();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mainBuffer.id);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, mainBuffer.binding, mainBuffer.id);
	glDispatchCompute(screenSize.x / groupSize.x, screenSize.y / groupSize.y + (screenSize.y % groupSize.y != 0), groupSize.z);
}

unsigned int ComputeShader::CreateProgram(std::string source)
{
	type = ShaderType::compute;

	id = glCreateProgram();
	unsigned int compute = CompileShader(GL_COMPUTE_SHADER, source);

	glAttachShader(id, compute);
	glLinkProgram(id);


	int success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		int length;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> infoLog(length);
		glGetProgramInfoLog(id, length, NULL, &infoLog[0]);
		std::cerr << "Error: program linking failed\n" << std::string(infoLog.begin(), infoLog.end()) << std::endl;
	}


	glValidateProgram(id);

	glDeleteShader(compute);
	GlErrorCheck();
	return id;
}

Texture::Texture(unsigned int id, int unitId) : id(id), unitId(unitId)
{
}

Texture::Texture() : id(-1), unitId(-1)
{
}
