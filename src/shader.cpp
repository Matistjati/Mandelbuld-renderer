#include "shader.h"
#include <Windows.h>
#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexCode = ParseShader(vertexPath);
	std::string fragmentCode = ParseShader(fragmentPath);

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();



	id = glCreateProgram();
	unsigned int vertex = CompileShader(GL_VERTEX_SHADER, vertexCode);
	unsigned int fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);

	char infoLog[256];

	int success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 256, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}


	glValidateProgram(id);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	glDeleteProgram(id);
}

void Shader::use()
{
	glUseProgram(id);
}

void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set3f(const std::string & name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void Shader::set4f(const std::string & name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
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
	char infoLog[256];
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{

		glGetShaderInfoLog(id, 256, NULL, infoLog);
		std::cout << "failed to compile " <<
			((type == GL_VERTEX_SHADER) ? " vertex " : " fragment ") << " shader.\nError: " << infoLog << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}
