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


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
	{
		Position = position;
		Yaw = yaw;
		Pitch = pitch;
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float yaw, float pitch) :  MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
	{
		Position = glm::vec3(posX, posY, posZ);
		Yaw = yaw;
		Pitch = pitch;
	}

	glm::mat2 GetYawMatrix2()
	{
		float rad = glm::radians(-Yaw);
		return glm::mat2(cos(rad), -sin(rad),
						 sin(rad), cos(rad));
	}

	glm::mat2 GetPitchMatrix2()
	{
		float rad = glm::radians(Pitch);
		return glm::mat2(cos(rad), -sin(rad),
						 sin(rad), cos(rad));
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		//std::cout << "x: " << Front.x << ", y: " << Front.y << ", z:" << Front.z << std::endl;
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
		{
			// Vertical movement
			Position.x += -cos(glm::radians(Pitch)) * velocity;

			glm::vec2 verticalFront(-1, 0);

			verticalFront = GetYawMatrix2() * verticalFront;

			// If we face up, we wanna move slower horizontally
			float rads2 = abs(fmod(glm::radians(Pitch), M_PI));
			float percentLookingMiddle = (rads2 < M_PI_2) ?
				rads2 / M_PI_2 :
				(M_PI - rads2) / M_PI_2;

			Position += glm::vec3(0, verticalFront.x, verticalFront.y) * velocity * percentLookingMiddle;
		}
		/*if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;*/
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = false)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
	}
};
#endif