#include "headers/Mandelbulb.h"
#include "glm.hpp"
#include "headers/shader.h"
#include "headers/camera.h"
#include <windows.h>
#include <glew.h>

Mandelbulb::Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera, glm::vec2 ScreenSize, Time time)
	: explorationShader(explorationShader), renderShader(renderShader), camera(camera), power(power), screenSize(ScreenSize), time(time)
{
	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);

	SetUniformLocations(renderShader);
	SetUniforms(renderShader);
}

Mandelbulb::Mandelbulb(float power, Shader & explorationShader, Shader & renderShader, Camera & camera) 
	: screenSize(glm::vec2(DefaultWidth, DefaultHeight)), time(Time()),	explorationShader(explorationShader), renderShader(renderShader), camera(camera), power(power)
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

	double xoffset = newPos.x - mouseOffset.x;
	double yoffset = mouseOffset.y - newPos.y; // reversed since y-coordinates go from bottom to top

	mouseOffset = newPos;

	camera.ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));

	explorationShader.setMat2(explorationShader.uniforms[Locations::pitchMatrix], camera.GetPitchMatrix2());
	explorationShader.setMat2(explorationShader.uniforms[Locations::yawMatrix], camera.GetYawMatrix2());
}

void Mandelbulb::SetUniforms(Shader &shader)
{
	shader.use();

	shader.setFloat(shader.uniforms[Locations::power], power);
	shader.set3f(shader.uniforms[Locations::eye], camera.position);
	shader.setMat2(shader.uniforms[Locations::yawMatrix], camera.GetYawMatrix2());
	shader.setMat2(shader.uniforms[Locations::pitchMatrix], camera.GetPitchMatrix2());
	shader.setMat2(shader.uniforms[Locations::rollMatrix], camera.GetRollMatrix2());
	shader.setInt("width", static_cast<int>(screenSize.x));
	shader.setInt("height", static_cast<int>(screenSize.y));
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
}