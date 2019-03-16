#pragma once

#ifndef CAMERA_H
#define CAMERA_H


#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "ToString.h"
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = 0;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;


// A camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// -1 or 1, for moving inside our outside of the bulb
	float movementReverse;

	// Camera Attributes
	glm::vec3 position;

	// Euler Angles
	float yaw;
	float pitch;

	// Camera options
	float MovementSpeed;
	float MouseSensitivity;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float Yaw = YAW, float Pitch = PITCH) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), movementReverse(-1)
	{
		position = position;
		yaw = Yaw;
		pitch = Pitch;
	}

	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float Yaw, float Pitch) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), movementReverse(-1)
	{
		position = glm::vec3(posX, posY, posZ);
		yaw = Yaw;
		pitch = Pitch;
	}

	glm::mat2 GetYawMatrix2()
	{
		float rad = glm::radians(-yaw);
		return glm::mat2(cos(rad), -sin(rad),
			sin(rad), cos(rad));
	}

	glm::mat2 GetPitchMatrix2()
	{
		float rad = glm::radians(pitch);
		return glm::mat2(cos(rad), -sin(rad),
			sin(rad), cos(rad));
	}

	glm::vec3 GetForwardVector()
	{
		glm::vec3 out;

		// Vertical movement
		out.x = cos(glm::radians(pitch)) * movementReverse;


		// Horizontal movement
		glm::vec2 verticalFront(1 * movementReverse, 0);

		verticalFront = GetYawMatrix2() * verticalFront;

		// If we face up, we wanna move slower horizontally
		float rads2 = static_cast<float>(abs(fmod(glm::radians(pitch), M_PI)));
		float percentLookingMiddle = static_cast<float>((rads2 < M_PI_2) ?
			rads2 / M_PI_2 :
			(M_PI - rads2) / M_PI_2);

		out += glm::vec3(0, verticalFront.x, verticalFront.y) * percentLookingMiddle;

		return glm::normalize(out);
	}

	glm::vec3 GetRightVector()
	{
		glm::vec3 worldUp = glm::vec3(-1, 0, 0);
		return glm::normalize(glm::cross(GetForwardVector(), worldUp));
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			position += GetForwardVector() * velocity;
		if (direction == BACKWARD)
			position -= GetForwardVector() * velocity;

		if (direction == RIGHT)
			position += GetRightVector() * velocity;
		if (direction == LEFT)
			position -= GetRightVector() * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		yaw += xoffset;
		pitch += yoffset;
	}
};
#endif