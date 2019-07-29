#include "headers/Shader.h"
#include <glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include "headers/Debug.h"
#include <vector>
#include <map>
#include "headers/FileManager.h"
#include "headers/Fractal.h"
#include <algorithm>
#include "headers/Fractal2D.h"

void Shader::use()
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
	GlErrorCheck();
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, bool path)
{
	type = fragment;
	std::string vShaderCode = path ? FileManager::ReadFile(vertexPath).c_str() : vertexPath.c_str();
	std::string fShaderCode = path ? FileManager::ReadFile(fragmentPath).c_str() : fragmentPath.c_str();

	id = glCreateProgram();
	unsigned int vertex = CompileShader(GL_VERTEX_SHADER, vShaderCode);
	unsigned int fragment = CompileShader(GL_FRAGMENT_SHADER, fShaderCode);

	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
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

	if (fShaderCode.find("<bufferType>") != std::string::npos)
	{
		Buffer buffer = GenerateBufferForProgram(fShaderCode);

		buffers[buffer.name] = buffer;
	}


	glDeleteShader(vertex);
	glDeleteShader(fragment);
	GlErrorCheck();
}

Shader::Shader(unsigned int id, ShaderType type) : id(id), type(type) {}


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

		std::cerr << "Failed to compile " << shaderTypeToString[type] << " shader.\nError: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;
		glDeleteShader(id);
		return -1;
	}

	return id;
}

Buffer Shader::GenerateBufferForProgram(std::string source)
{
	size_t bufferTypeStart = source.find("<bufferType>");
	// TODO: multiple buffers
	if (bufferTypeStart != std::string::npos)
	{
		bufferTypeStart += (std::string("<bufferType>")).length();
		std::string type = source.substr(bufferTypeStart, source.find("</bufferType>") - bufferTypeStart);

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

		int viewPort[4];
		glGetIntegerv(GL_VIEWPORT, &viewPort[0]);

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, viewPort[2] * viewPort[3] * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);


		GlErrorCheck();
		return Buffer(buffer, binding, type);
	}
}

void Shader::SetUniform(const Uniform<float> value) const
{
	glUniform1f(value.id, value.value);
}

void Shader::SetUniform(Uniform<int> value) const
{
	glUniform1i(value.id, value.value);
}

void Shader::SetUniform(Uniform<bool> value) const
{
	glUniform1i(value.id, value.value);
}

void Shader::SetUniform(Uniform<glm::ivec2> vector) const
{
	glUniform2f(vector.id, (float)vector.value.x, (float)vector.value.y);
}

void Shader::SetUniform(Uniform<glm::vec2> vector) const
{
	glUniform2f(vector.id, vector.value.x, vector.value.y);
}

void Shader::SetUniform(Uniform<glm::vec3> vector) const
{
	glUniform3f(vector.id, vector.value.x, vector.value.y, vector.value.z);
}

void Shader::SetUniform(const Uniform<Time> value) const
{
	glUniform1f(value.id, static_cast<float>(value.value.GetTotalTime()));
}

void Shader::SetUniform(Uniform<glm::vec4> vector) const
{
	glUniform4f(vector.id, vector.value.x, vector.value.y, vector.value.z, vector.value.w);
}

void Shader::SetUniform(Uniform<glm::mat2> &mat) const
{
	glUniformMatrix2fv(mat.id, 1, GL_FALSE, &mat.value[0][0]);
}

void Shader::SetUniform(Uniform<glm::mat3>& mat) const
{
	glUniformMatrix3fv(mat.id, 1, GL_FALSE, &mat.value[0][0]);
}

void Shader::SetUniform(unsigned int id, float x, float y, float z) const
{
	glUniform3f(id, x, y, z);
}

void Shader::SetUniform(unsigned int id, int value) const
{
	glUniform1i(id, value);
}

void Shader::SetUniformStr(Uniform<glm::vec2> vector) const
{
	glUniform2f(glGetUniformLocation(id, vector.name.c_str()), vector.value.x, vector.value.y);
}

void Shader::SetUniformStr(Uniform<glm::vec3> vector) const
{
	glUniform3f(glGetUniformLocation(id, vector.name.c_str()), vector.value.x, vector.value.y, vector.value.z);
}

void Shader::SetUniformStr(Uniform<glm::vec4> vector) const
{
	glUniform4f(glGetUniformLocation(id, vector.name.c_str()), vector.value.x, vector.value.y, vector.value.z, vector.value.w);
}

void Shader::SetUniformStr(Uniform<glm::mat2> &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(id, mat.name.c_str()), 1, GL_FALSE, &mat.value[0][0]);
}

void Shader::SetUniformStr(Uniform<glm::mat3> &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(id, mat.name.c_str()), 1, GL_FALSE, &mat.value[0][0]);
}

void Shader::SetUniformStr(Uniform<glm::mat4> &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, mat.name.c_str()), 1, GL_FALSE, &mat.value[0][0]);
}

void Shader::SetUniformStr(Uniform<bool> value) const
{
	glUniform1i(glGetUniformLocation(id, value.name.c_str()), value.value);
}

void Shader::SetUniformStr(Uniform<int> value) const
{
	glUniform1i(glGetUniformLocation(id, value.name.c_str()), value.value);
}

void Shader::SetUniformStr(Uniform<float> value) const
{
	glUniform1f(glGetUniformLocation(id, value.name.c_str()), value.value);
}

void Shader::SetUniformStr(const std::string & name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void Shader::SetUniformStr(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void Shader::SetUniformStr(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void Shader::SetUniformStr(const std::string& name, glm::vec3 vector) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), vector.x, vector.y, vector.z);
}

ComputeShader::ComputeShader(const std::string& computePath, bool path, glm::ivec3 groupSize)
	: Shader(CreateProgram(path ? FileManager::ReadFile(computePath) : computePath), ShaderType::compute), groupSize(groupSize)
{
	glUseProgram(id);
	std::string source = path ? FileManager::ReadFile(computePath) : computePath;
	Buffer buffer = GenerateBufferForProgram(source);
	
	if (buffer.name == "mainBuffer")
	{
		mainBuffer = buffer;
	}
	else
	{
		buffers[buffer.name] = buffer;
	}

}

void ComputeShader::Invoke(glm::ivec2 screenSize)
{
	this->use();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mainBuffer.id);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, mainBuffer.binding, mainBuffer.id);
	glDispatchCompute(screenSize.x / groupSize.x + (screenSize.x % groupSize.x != 0), screenSize.y / groupSize.y + (screenSize.y % groupSize.y != 0), groupSize.z);
}

unsigned int ComputeShader::CreateProgram(std::string source)
{
	type = compute;

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
