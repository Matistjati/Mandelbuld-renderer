#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <string>


class Shader
{
public:
	// the program ID
	unsigned int id;

	// constructor reads and builds the shader
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();
	// use/activate the shader
	void use();
	// utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;

private:
	std::string ParseShader(const std::string& file);
	unsigned int CompileShader(unsigned int type, const std::string& source);
};

#endif