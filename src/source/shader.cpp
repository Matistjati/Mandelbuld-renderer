#include "headers/Shader.h"
#include <glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include "headers/Debug.h"
#include <vector>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, bool path)
{
	const char* vShaderCode;
	const char* fShaderCode;
	if (path)
	{
		std::string vertexCode = ParseShader(vertexPath);
		std::string fragmentCode = ParseShader(fragmentPath);

		vShaderCode = vertexCode.c_str();
		fShaderCode = fragmentCode.c_str();
	}
	else
	{
		vShaderCode = vertexPath.c_str();
		fShaderCode = fragmentPath.c_str();
	}




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
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> infoLog(length);
		glGetProgramInfoLog(id, length, NULL, &infoLog[0]);
		std::cerr << "Error: program linking failed\n" << std::string(infoLog.begin(), infoLog.end()) << std::endl;
	}


	glValidateProgram(id);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	GlErrorCheck();
}

Shader::~Shader()
{
	glDeleteProgram(id);
	GlErrorCheck();
}

void Shader::use()
{
	glUseProgram(id);
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
	glUniform1f(value.id, value.value.GetTotalTime());
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

std::string Shader::ParseShader(const std::string& file)
{
	std::ifstream stream;

	stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	std::string str;
	try
	{
		stream.open(file);

		str = std::string((std::istreambuf_iterator<char>(stream)),
			std::istreambuf_iterator<char>());
	}
	catch (std::system_error& e)
	{
		std::cerr << e.code().message() << std::endl;
	}


	return str;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
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

		std::cerr << "Failed to compile " <<
			((type == GL_VERTEX_SHADER) ? " vertex " : " fragment ") << " shader.\nError: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;
		glDeleteShader(id);
		return -1;
	}

	return id;
}
