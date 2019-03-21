#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm.hpp>

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
	void setFloat(int location, float value) const;
	void set3f(const std::string &name, float x, float y, float z) const;
	void set3f(int location, float x, float y, float z) const;
	void set3f(const std::string &name, glm::vec3 vector) const;
	void set3f(int location, glm::vec3 vector) const;
	void set4f(const std::string &name, float x, float y, float z, float w) const;
	void setMat2(const std::string &name, const glm::mat2 &mat) const;
	void setMat2(int location, const glm::mat2 &mat) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
	std::string ParseShader(const std::string& file);
	unsigned int CompileShader(unsigned int type, const std::string& source);
};

#endif