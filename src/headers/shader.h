#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <glm.hpp>
#include <map>
#include "headers/Uniform.h"
#include "headers/Time.h"
#include "headers/Debug.h"
#include <nanogui/nanogui.h>
#include <map>
#include <vector>

enum class ShaderType
{
	fragment, compute
};

struct Buffer
{
public:
	enum class BufferType
	{
		vertexArray,
		buffer,
		none
	};
	Buffer(unsigned int id, int binding, BufferType type, std::string name) : id(id), binding(binding), type(type), name(name), timesToClear(0), resetFrame() {}
	Buffer(unsigned int id, int binding, std::string name) : id(id), binding(binding), type(BufferType::buffer), name(name), timesToClear(0), resetFrame() {}
	Buffer(unsigned int id, BufferType type, std::string name) : id(id), binding(-1), type(type), name(name), timesToClear(0), resetFrame() {}
	Buffer(unsigned int id, int binding, BufferType type) : id(id), binding(binding), type(type), timesToClear(0), resetFrame() {}
	Buffer(unsigned int id, std::string name) : id(id), binding(-1), type(BufferType::buffer), name(name), timesToClear(0), resetFrame() {}
	Buffer(unsigned int id, int binding) : id(id), binding(binding), type(BufferType::buffer), timesToClear(0), resetFrame() {}
	Buffer(unsigned int id, BufferType type) : id(id), binding(-1), type(type), timesToClear(0), resetFrame() {}
	Buffer(unsigned int id) : id(id), binding(-1), type(BufferType::buffer), timesToClear(0), resetFrame() {}
	Buffer() : id(-1), binding(-1), type(BufferType::none), timesToClear(0), resetFrame() {}
	BufferType type;
	unsigned int id;
	int binding;
	std::string name;
	float timesToClear;
	bool resetFrame;

	void Delete()
	{
		if (glIsBuffer(id))
		{
			if (type == BufferType::vertexArray) glDeleteVertexArrays(1, &id);
			else if (type == BufferType::buffer) glDeleteBuffers(1, &id);
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
	Shader(const std::string& vertexPath, const std::string& fragmentPath, bool path = false);
	Shader(unsigned int id, ShaderType type);
	~Shader();
	// use/activate the shader
	void Use();
	// utility uniform functions
	void SetUniform(Uniform<float> value) const;
	void SetUniform(Uniform<float> value, bool renderMode) const;
	void SetUniform(Uniform<int> value) const;
	void SetUniform(Uniform<int> value, bool renderMode) const;
	void SetUniform(Uniform<unsigned int> value) const;
	void SetUniform(Uniform<bool> value) const;
	void SetUniform(Uniform<bool> value, bool renderMode) const;
	void SetUniform(Uniform<glm::ivec2> vector) const;
	void SetUniform(Uniform<glm::vec2> vector) const;
	void SetUniform(Uniform<glm::vec2> vector, bool renderMode) const;
	void SetUniform(Uniform<glm::vec3> vector) const;
	void SetUniform(Uniform<glm::vec3> vector, bool renderMode) const;
	void SetUniform(Uniform<glm::vec4> vector) const;
	void SetUniform(Uniform<glm::vec4> vector, bool renderMode) const;
	void SetUniform(Uniform<glm::mat2> &mat) const;
	void SetUniform(Uniform<glm::mat3> &mat) const;
	void SetUniform(Uniform<glm::mat4> &mat) const;
	void SetUniform(Uniform<nanogui::Color> &color) const;
	void SetUniform(Uniform<nanogui::Color> &color, bool renderMode) const;
	void SetUniform(Uniform<Time> time) const;
	void SetUniform(Uniform<Time> time, bool renderMode) const;
	void SetUniform(unsigned int id, float x, float y, float z) const;
	void SetUniform(unsigned int id, float value) const;
	void SetUniform(unsigned int id, int value) const;
	void SetUniformStr(Uniform<glm::vec2> vector) const;
	void SetUniformStr(Uniform<glm::vec3> vector) const;
	void SetUniformStr(Uniform<glm::vec4> vector) const;
	void SetUniformStr(Uniform<glm::mat2> &mat) const;
	void SetUniformStr(Uniform<glm::mat3> &mat) const;
	void SetUniformStr(Uniform<glm::mat4> &mat) const;
	void SetUniformStr(Uniform<bool> value) const;
	void SetUniformStr(Uniform<int> value) const;
	void SetUniformStr(Uniform<unsigned int> value) const;
	void SetUniformStr(Uniform<float> value) const;
	void SetUniformStr(const std::string &name, int x) const;
	void SetUniformStr(const std::string &name, float x) const;
	void SetUniformStr(const std::string &name, float x, float y) const;
	void SetUniformStr(const std::string &name, float x, float y, float z) const;
	void SetUniformStr(const std::string &name, float x, float y, float z, float w) const;
	void SetUniformStr(const std::string &name, glm::vec3 vector) const;

protected:
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateFragmentProgram(const std::string& vertex, const std::string& fragment);
	std::vector<Buffer> GenerateBuffersForProgram(std::string source);

};

class ComputeShader : public Shader
{
public:
	const static int DefaultRenderingFrequency = 1;

	void UseRender();
	ComputeShader(const std::string& computePath, std::string vertexPath, std::string renderPath, bool path, glm::ivec3 groupSize, int renderingFrequency);
	void Invoke(glm::ivec2 screenSize);

	unsigned int renderId;
	Buffer mainBuffer;
	glm::ivec3 groupSize;
	int renderingFrequency;
	std::map<std::string, unsigned int> uniformRenderIds;

protected:
	unsigned int CreateProgram(std::string source);
};

#endif