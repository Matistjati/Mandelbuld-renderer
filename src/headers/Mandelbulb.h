#pragma once
#ifndef MANDELBULB_H
#define MANDELBULB_H

#include "shader.h"
#include "Time.h"
#include "Camera.h"
#include "headers/Fractal.h"
#include "headers/Fractal3d.h"

class Mandelbulb : public Fractal3D
{
public:
	static const int defaultPower = 8;

	Mandelbulb(float power, Camera &camera, glm::vec3 sun, glm::ivec2 screenSize, Time time);
	Mandelbulb(float power, Camera &camera, glm::vec3 sun);
	Mandelbulb(float power, Camera &camera);
	Mandelbulb();
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void SetUniforms(Shader& shader) override;
	void SetUniformLocations(Shader& shader) override;
	void SetUniformNames() override;
	void Update() override;
	static Shader& GenerateShader(bool highQuality);

	Uniform<float> power;
	Uniform<float> genericParameter;
private:
	static constexpr const char* SourcePath = "resources/shaders/3D/Mandelbulb.fs";
};

#endif
