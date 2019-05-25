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
	static const int defaultPower = 1;

	Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera, glm::vec3 sun, glm::ivec2 screenSize, Time time);
	Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera, glm::vec3 sun);
	Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera);
	Mandelbulb(Shader &explorationShader, Shader &renderShader);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void SetUniforms(Shader& shader) override;
	void SetUniformLocations(Shader& shader) override;
	void SetUniformNames() override;
	void Update() override;


	Uniform<float> power;
	Uniform<float> genericParameter;
	Uniform<glm::vec3> sun;


};

#endif
