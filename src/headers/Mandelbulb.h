#pragma once
#ifndef MANDELBULB_H
#define MANDELBULB_H

#include "shader.h"
#include "Time.h"
#include "Camera.h"
#include "headers/Fractal.h"

class Mandelbulb : public Fractal
{
public:
	static const int defaultPower = 8;

	Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera, glm::vec3 sun, glm::ivec2 screenSize, Time time);
	Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera, glm::vec3 sun);
	Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera);
	Mandelbulb(Shader &explorationShader, Shader &renderShader);
	void MouseCallback(GLFWwindow* window, double x, double y) override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void FramebufferSizeCallback(GLFWwindow * window, int width, int height) override;
	void SetUniforms(Shader& shader) override;
	void SetUniformLocations(Shader& shader) override;
	void SetUniformNames() override;
	void Update() override;
	void SaveImage(const std::string path) override;


	Camera &camera;
	Uniform<float> power;
	Time time;
	Uniform<float> genericParameter;
	Uniform<glm::vec3> sun;

private:
	glm::vec2 mouseOffset;
	bool firstMouse = true;
};

#endif
