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
	unsigned int *uniforms;

	// constructor reads and builds the shader
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();
	// use/activate the shader
	void use();
	// utility uniform functions
	void SetBool(const std::string &name, bool value) const;
	void SetInt(const std::string &name, int value) const;
	void SetFloat(const std::string &name, float value) const;
	void SetFloat(int location, float value) const;
	void Set3f(const std::string &name, float x, float y, float z) const;
	void Set3f(int location, float x, float y, float z) const;
	void Set3f(const std::string &name, glm::vec3 vector) const;
	void Set3f(int location, glm::vec3 vector) const;
	void Set4f(const std::string &name, float x, float y, float z, float w) const;
	void SetMat2(const std::string &name, const glm::mat2 &mat) const;
	void SetMat2(int location, const glm::mat2 &mat) const;
	void SetMat4(const std::string &name, const glm::mat4 &mat) const;

private:
	std::string ParseShader(const std::string& file);
	unsigned int CompileShader(unsigned int type, const std::string& source);
};

#endif