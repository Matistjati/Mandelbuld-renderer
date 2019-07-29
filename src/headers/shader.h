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
	enum BufferType
	{
		vertexArray,
		buffer,
		none
	};
	Buffer(unsigned int id, int binding, BufferType type, std::string name) : id(id), binding(binding), type(type), name(name) {}
	Buffer(unsigned int id, int binding, std::string name) : id(id), binding(binding), type(buffer), name(name) {}
	Buffer(unsigned int id, BufferType type, std::string name) : id(id), binding(-1), type(type), name(name) {}
	Buffer(unsigned int id, int binding, BufferType type) : id(id), binding(binding), type(type) {}
	Buffer(unsigned int id, std::string name) : id(id), binding(-1), type(buffer), name(name) {}
	Buffer(unsigned int id, int binding) : id(id), binding(binding), type(buffer) {}
	Buffer(unsigned int id, BufferType type) : id(id), binding(-1), type(type) {}
	Buffer(unsigned int id) : id(id), binding(-1), type(buffer) {}
	Buffer() : id(-1), binding(-1), type(none) {}
	BufferType type;
	unsigned int id;
	int binding;
	std::string name;

	void Delete()
	{
		if (glIsBuffer(id))
		{
			if (type == vertexArray) glDeleteVertexArrays(1, &id);
			else if (type == buffer) glDeleteBuffers(1, &id);
		}
	}
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
	Shader(unsigned int id, ShaderType type);
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

protected:
	unsigned int CompileShader(unsigned int type, const std::string& source);
	Buffer GenerateBufferForProgram(std::string source);
};

class ComputeShader : public Shader
{
public:
	Buffer mainBuffer;
	glm::ivec3 groupSize;
	ComputeShader(const std::string& computePath, bool path, glm::ivec3 groupSize);
	void Invoke(glm::ivec2 screenSize);
protected:
	unsigned int CreateProgram(std::string source);
};

#endif