#include "headers/Mandelbulb.h"
#include "glm.hpp"
#include "headers/shader.h"
#include "headers/camera.h"
#include <windows.h>
#include <glew.h>
#include <iostream>

Mandelbulb::Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera, glm::vec3 sun, glm::ivec2 screenSize, Time time)
	: explorationShader(explorationShader), renderShader(renderShader), camera(camera), power(power), screenSize(screenSize), time(time), genericParameter(1), sun(sun)
{
	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);

	SetUniformLocations(renderShader);
	SetUniforms(renderShader);
}

Mandelbulb::Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera, glm::vec3 sun)
	: screenSize(glm::dvec2(DefaultWidth, DefaultHeight)), time(Time()), explorationShader(explorationShader), renderShader(renderShader), camera(camera), power(power), genericParameter(1), sun(sun)
{
	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);

	SetUniformLocations(renderShader);
	SetUniforms(renderShader);
}

Mandelbulb::Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera)
	: screenSize(glm::dvec2(DefaultWidth, DefaultHeight)), time(Time()), explorationShader(explorationShader), renderShader(renderShader), camera(camera), power(power), genericParameter(1), sun(glm::normalize(glm::dvec3(0.577, 0.577, 0.577)))
{
	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);

	SetUniformLocations(renderShader);
	SetUniforms(renderShader);
}


void Mandelbulb::ProcessMouse(glm::vec2 newPos)
{
	if (firstMouse)
	{
		mouseOffset = newPos;
		firstMouse = false;
	}

	camera.ProcessMouseMovement(static_cast<float>(newPos.x - mouseOffset.x),
		static_cast<float>(mouseOffset.y - newPos.y) // reversed since y-coordinates go from bottom to top
	);

	mouseOffset = newPos;

	explorationShader.SetMat2(explorationShader.uniforms[Locations::pitchMatrix], camera.GetPitchMatrix2());
	explorationShader.SetMat2(explorationShader.uniforms[Locations::yawMatrix], camera.GetYawMatrix2());
}

void Mandelbulb::SetUniforms(Shader & shader)
{
	shader.use();

	shader.SetFloat(shader.uniforms[Locations::power], power);
	shader.Set3f(shader.uniforms[Locations::eye], camera.position);
	shader.Set3f(shader.uniforms[Locations::sun], sun);
	shader.SetMat2(shader.uniforms[Locations::yawMatrix], camera.GetYawMatrix2());
	shader.SetMat2(shader.uniforms[Locations::pitchMatrix], camera.GetPitchMatrix2());
	shader.SetMat2(shader.uniforms[Locations::rollMatrix], camera.GetRollMatrix2());
	shader.SetInt("width", static_cast<int>(screenSize.x));
	shader.SetInt("height", static_cast<int>(screenSize.y));
	shader.SetInt("worldFlip", camera.worldFlip);

#if _DEBUG
	if (glGetError() != GL_NO_ERROR)
	{
		std::cout << "Opengl error: " << std::to_string(glGetError()) << std::endl;
	}
#endif
}

void Mandelbulb::SetUniformLocations(Shader &shader)
{
	shader.uniforms = new unsigned int[Locations::count];
	shader.uniforms[Locations::pitchMatrix] = glGetUniformLocation(shader.id, "pitchMatrix");
	shader.uniforms[Locations::rollMatrix]	= glGetUniformLocation(shader.id, "rollMatrix");
	shader.uniforms[Locations::yawMatrix]	= glGetUniformLocation(shader.id, "yawMatrix");
	shader.uniforms[Locations::power]		= glGetUniformLocation(shader.id, "power");
	shader.uniforms[Locations::eye]			= glGetUniformLocation(shader.id, "eye");
	shader.uniforms[Locations::sun]			= glGetUniformLocation(shader.id, "sun");

#if _DEBUG
	if (glGetError() != GL_NO_ERROR)
	{
		std::cout << "Opengl error: " << std::to_string(glGetError()) << std::endl;
	}
#endif
}
