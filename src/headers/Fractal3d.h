#pragma once
#ifndef FRACTAL3D_H
#define FRACTAL3D_H

#include "headers/Fractal.h"
#include "headers/Camera.h"
#include "headers/Time.h"

class Fractal3D : public Fractal
{
public:
	Camera& camera;
	Time time;

	Fractal3D(Shader& explorationShader, Shader& renderShader, Camera& camera, glm::ivec2 screenSize, Time time);
	Fractal3D(Shader& explorationShader, Shader& renderShader);


	void MouseCallback(GLFWwindow* window, double x, double y) override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height) override;
	void SetUniforms(Shader& shader) override;
	void SetUniformLocations(Shader& shader) override;
	void SetUniformNames() override;
	void SaveImage(const std::string path) override;
	void Update() override;


private:
	glm::vec2 mouseOffset;
	bool firstMouse = true;
};

#endif