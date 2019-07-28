#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm.hpp>
#include "headers/Uniform.h"
#include "headers/Time.h"
#include <map>

template<typename T>
struct Uniform;

enum ShaderType
{
	fragment, compute
};

struct Buffer
{
public:
	Buffer(int id, int binding) : id(id), binding(binding) {}
	Buffer(int id) : id(id), binding(-1) {}
	int id;
	int binding;
};

class Shader
{
public:
	// the program ID
	unsigned int id;
	ShaderType type;
	std::map<std::string, Buffer> buffers;

	// constructor reads and builds the shader
	Shader(const std::string& vertexPath, const std::string& fragmentPath, bool path);
	Shader(const std::string& computePath, bool path);
	~Shader();
	// use/activate the shader
	void use();
	// utility uniform functions
	void SetUniform(Uniform<float> value) const;
	void SetUniform(Uniform<int> value) const;
	void SetUniform(Uniform<bool> value) const;
	void SetUniform(Uniform<glm::ivec2> vector) const;
	void SetUniform(Uniform<glm::vec2> vector) const;
	void SetUniform(Uniform<glm::vec3> vector) const;
	void SetUniform(Uniform<glm::vec4> vector) const;
	void SetUniform(Uniform<glm::mat2> &mat) const;
	void SetUniform(Uniform<glm::mat3> &mat) const;
	void SetUniform(Uniform<Time> time) const;
	void SetUniform(unsigned int id, float x, float y, float z) const;
	void SetUniform(unsigned int id, int value) const;
	void SetUniformStr(Uniform<glm::vec2> vector) const;
	void SetUniformStr(Uniform<glm::vec3> vector) const;
	void SetUniformStr(Uniform<glm::vec4> vector) const;
	void SetUniformStr(Uniform<glm::mat2> &mat) const;
	void SetUniformStr(Uniform<glm::mat3> &mat) const;
	void SetUniformStr(Uniform<glm::mat4> &mat) const;
	void SetUniformStr(Uniform<bool> value) const;
	void SetUniformStr(Uniform<int> value) const;
	void SetUniformStr(Uniform<float> value) const;
	void SetUniformStr(const std::string &name, float x, float y) const;
	void SetUniformStr(const std::string &name, float x, float y, float z) const;
	void SetUniformStr(const std::string &name, float x, float y, float z, float w) const;
	void SetUniformStr(const std::string &name, glm::vec3 vector) const;

private:
	unsigned int CompileShader(unsigned int type, const std::string& source);
};

#endif