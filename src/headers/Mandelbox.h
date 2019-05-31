#pragma once
#ifndef Mandelbox_H
#define Mandelbox_H

#include "shader.h"
#include "Time.h"
#include "Camera.h"
#include "headers/Fractal.h"
#include "headers/Fractal3d.h"

class Mandelbox : public Fractal3D
{
public:
	static const int defaultPower = 1;

	Mandelbox(float power, Camera& camera, glm::vec3 sun, glm::ivec2 screenSize, Time time);
	Mandelbox(float power, Camera& camera, glm::vec3 sun);
	Mandelbox(float power, Camera& camera);
	Mandelbox();
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void SetUniforms(Shader& shader) override;
	void SetUniformLocations(Shader& shader) override;
	void SetUniformNames() override;
	void Update() override;
	static Shader& GenerateShader(bool highQuality);

	Uniform<float> power;
	Uniform<float> genericParameter;
private:
	static constexpr const char* SourcePath = "resources/shaders/3D/Mandelbox.fs";
};

#endif
