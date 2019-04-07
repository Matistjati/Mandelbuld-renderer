#pragma once
#ifndef MANDELBULB_H
#define MANDELBULB_H

#include "shader.h"
#include "Time.h"
#include "Camera.h"

enum Locations
{
	yawMatrix   = 0,
	pitchMatrix = 1,
	rollMatrix  = 2,
	eye         = 3,
	power       = 4,
	sun         = 5,
	count		= sun + 1,
}; 


class Mandelbulb
{
public:
	static const int defaultPower = 8;
	static const int DefaultWidth = 1080;
	static const int DefaultHeight = 1080;

	Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera, glm::vec2 screenSize, Time time);
	Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera);
	void ProcessMouse(glm::vec2 offset);
	void SetUniforms(Shader &shader);
	void SetUniformLocations(Shader &shader);

	Camera &camera;
	Shader &explorationShader;
	Shader &renderShader;
	float power;
	glm::ivec2 screenSize;
	Time time;
	float genericParameter;

private:
	glm::vec2 mouseOffset;
	bool firstMouse = true;
};

#endif
