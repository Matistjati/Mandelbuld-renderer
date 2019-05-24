#include <windows.h>
#include "headers/Mandelbulb.h"
#include "glm.hpp"
#include "headers/shader.h"
#include "headers/camera.h"
#include <glew.h>
#include <iostream>
#include "headers/Image.h"

Mandelbulb::Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera, glm::vec3 sun, glm::ivec2 screenSize, Time time)
	: Fractal(explorationShader, renderShader, screenSize), camera(camera), power(power), time(time), genericParameter(1), sun(sun)
{
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
}

Mandelbulb::Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera, glm::vec3 sun)
	: Fractal(explorationShader, renderShader, screenSize), time(Time()), camera(camera), power(power), genericParameter(1), sun(sun)
{
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
}

Mandelbulb::Mandelbulb(float power, Shader &explorationShader, Shader &renderShader, Camera &camera)
	: Fractal(explorationShader, renderShader, glm::ivec2(DefaultWidth, DefaultHeight)), time(Time()), camera(camera), power(power), genericParameter(1), sun(glm::normalize(glm::dvec3(0.577, 0.577, 0.577)))
{
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
}

Mandelbulb::Mandelbulb(Shader & explorationShader, Shader & renderShader)
	: Fractal(explorationShader, renderShader, glm::ivec2(DefaultWidth, DefaultHeight)), power(defaultPower), time(Time()), genericParameter(1), sun(glm::normalize(glm::dvec3(0.577, 0.577, 0.577))),
	camera(*(new Camera(glm::vec3(1.8f, 0.8f, -0.6f), // Position
		169, -14, 0.001f, // Yaw, pitch, roll
		0.1f, 3, 200))) // mouseSensitivity, movementSpeed, rollSpeed
{
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
}

void Mandelbulb::MouseCallback(GLFWwindow * window, double x, double y)
{
	glm::vec2 newPos = glm::vec2(x, y);
	if (firstMouse)
	{
		mouseOffset = newPos;
		firstMouse = false;
	}

	camera.ProcessMouseMovement(static_cast<float>(newPos.x - mouseOffset.x),
		static_cast<float>(mouseOffset.y - newPos.y) // reversed since y-coordinates go from bottom to top
	);

	mouseOffset = newPos;

	explorationShader.SetUniform(camera.pitchMatrix);
	explorationShader.SetUniform(camera.yawMatrix);
}

void Mandelbulb::KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_REPEAT && action != GLFW_PRESS)
	{
		return;
	}

	if (key == GLFW_KEY_S && (mods && GLFW_MOD_CONTROL) == 1)
	{
		SaveImage("image.png");
		return;
	}
	glGetError();
	switch (key)
	{
		// Exit
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, 1);
		break;

		// WASD movement
	case GLFW_KEY_W:
		camera.ProcessMovement(Camera_Movement::forward, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;
	case GLFW_KEY_S:
		camera.ProcessMovement(Camera_Movement::back, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;
	case GLFW_KEY_A:
		camera.ProcessMovement(Camera_Movement::left, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;
	case GLFW_KEY_D:
		camera.ProcessMovement(Camera_Movement::right, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;

		// Up and down
	case GLFW_KEY_SPACE:
		camera.ProcessMovement(Camera_Movement::up, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;
	case GLFW_KEY_LEFT_SHIFT:
		camera.ProcessMovement(Camera_Movement::down, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;

		// World stuff
	case GLFW_KEY_Z:
		camera.worldFlip.value *= -1;
		explorationShader.SetUniform(camera.worldFlip);
		break;
	case GLFW_KEY_X:
		camera.movementReverse *= -1;
		break;


		// Changing the power of the fractal
	case GLFW_KEY_C:
		power.value += 0.03f;
		explorationShader.SetUniform(power);
		break;
	case GLFW_KEY_V:
		power.value -= 0.03f;
		explorationShader.SetUniform(power);
		break;


		// Camera roll
	case GLFW_KEY_Q:
		camera.ProcessRoll(static_cast<float>(camera.rollSpeed * time.deltaTime));
		explorationShader.SetUniform(camera.rollMatrix);
		break;
	case GLFW_KEY_E:
		camera.ProcessRoll(-static_cast<float>(camera.rollSpeed * time.deltaTime));
		explorationShader.SetUniform(camera.rollMatrix);
		break;


		// Debugging
		/*
	case GLFW_KEY_R:
		mandel->genericParameter += 0.1f;
		mandel->shader.setFloat("genericParameter", mandel->genericParameter);
		break;
	case GLFW_KEY_F:
		mandel->genericParameter -= 0.1f;
		mandel->shader.setFloat("genericParameter", mandel->genericParameter);
		break;
		*/

	default:
		break;
	}
#if _DEBUG
	int err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "Opengl error: " << glewGetErrorString(err) << std::endl;
	}
#endif
}


void Mandelbulb::FramebufferSizeCallback(GLFWwindow * window, int width, int height)
{
	screenSize.value = glm::ivec2(width, height);
	Fractal::explorationShader.SetUniform(screenSize.id, width);
	Fractal::explorationShader.SetUniform(screenSize.id, height);

	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void Mandelbulb::SetUniforms(Shader& shader)
{
	shader.use();

	shader.SetUniform(power);
	shader.SetUniform(sun);
	shader.SetUniform(camera.position);
	shader.SetUniform(camera.yawMatrix);
	shader.SetUniform(camera.pitchMatrix);
	shader.SetUniform(camera.rollMatrix);
	shader.SetUniform(screenSize);
	shader.SetUniform(camera.worldFlip);

#if _DEBUG
	if (glGetError() != GL_NO_ERROR)
	{
		std::cout << "Opengl error: " << std::to_string(glGetError()) << std::endl;
	}
#endif
}

void Mandelbulb::SetUniformLocations(Shader &shader)
{
	camera.pitchMatrix.id = glGetUniformLocation(shader.id, camera.pitchMatrix.name.c_str());
	camera.rollMatrix.id = glGetUniformLocation(shader.id, camera.rollMatrix.name.c_str());
	camera.yawMatrix.id = glGetUniformLocation(shader.id, camera.yawMatrix.name.c_str());
	camera.position.id = glGetUniformLocation(shader.id, camera.position.name.c_str());
	camera.worldFlip.id = glGetUniformLocation(shader.id, camera.worldFlip.name.c_str());
	power.id = glGetUniformLocation(shader.id, power.name.c_str());
	sun.id = glGetUniformLocation(shader.id, sun.name.c_str());
	screenSize.id = glGetUniformLocation(shader.id, screenSize.name.c_str());
	genericParameter.id = glGetUniformLocation(shader.id, genericParameter.name.c_str());

#if _DEBUG
	if (glGetError() != GL_NO_ERROR)
	{
		std::cout << "Opengl error: " << std::to_string(glGetError()) << std::endl;
	}
#endif
}

void Mandelbulb::SetUniformNames()
{
	camera.pitchMatrix.name = "pitchMatrix";
	camera.yawMatrix.name = "yawMatrix";
	camera.rollMatrix.name = "rollMatrix";
	camera.position.name = "eye";
	camera.worldFlip.name = "worldFlip";
	sun.name = "sun";
	power.name = "power";
	genericParameter.name = "genericParameter";
	screenSize.name = "screenSize";
}

void Mandelbulb::Update()
{
	time.PollTime();


	// Move sun in shader
	double time = glfwGetTime();
	sun.value = glm::normalize(glm::vec3(sin(time * 0.25),
		std::abs(sin(time * 0.1)) * -1,
		cos(time * 0.25)));

	explorationShader.SetUniform(sun);
}

void Mandelbulb::SaveImage(const std::string path)
{
	renderShader.use();
	SetUniformLocations(renderShader);
	SetUniforms(renderShader);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	Pixel* data = (Pixel*)malloc(screenSize.value.x * screenSize.value.y * 4);
	glReadPixels(0, 0, screenSize.value.x, screenSize.value.y, GL_RGBA, GL_UNSIGNED_BYTE, data);


	Image image(screenSize.value.x, screenSize.value.y, data);
	image.FlipVertically();
	image.Rotate180();


	// TODO: system for not overwriting files
	image.Save(path.c_str());

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	explorationShader.use();
}
