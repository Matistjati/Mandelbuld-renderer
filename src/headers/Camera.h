#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
	forward,
	back,
	left,
	right,
	up,
	down
};

// Default camera values
const float YAW = 0;
const float PITCH = 0.0f;
const float ROLL = 0.0f;
const float SPEED = 2.5f;
const float ROLLSPEED = 1.0f;
const float SENSITIVITY = 0.1f;

// A camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// -1 or 1, for moving inside our outside of the bulb
	float movementReverse;

	// -1 or 1, flipping the world upside up
	int worldFlip;

	// Camera Attributes
	glm::vec3 position;

	// Euler Angles
	float yaw;
	float pitch;
	float roll;

	// Camera options
	float movementSpeed;
	float mouseSensitivity;
	float rollSpeed;

	// Used for orientation
	const glm::vec3 worldUp = glm::vec3(0, 1, 0);
	
	// Constructor with vectors
	Camera(glm::vec3 Position, float Yaw, float Pitch, float Roll);
	Camera(glm::vec3 Position, float Yaw, float Pitch, float Roll, float mouseSensitivity, float movementSpeed, float rollSpeed);

	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float Yaw, float Pitch, float roll);

	// Rotation on the x axis
	glm::mat2 GetYawMatrix2();

	// Rotation on the y axis
	glm::mat2 GetPitchMatrix2();

	// Rotation on the z axis
	glm::mat2 GetRollMatrix2();

	glm::vec3 GetWorldUp();

	glm::vec3 GetWorldForward();

	// For moving forward or backward
	glm::vec3 GetForwardVector();

	// For moving left or right
	glm::vec3 GetRightVector();

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessMovement(Camera_Movement direction, float deltaTime);

	// Processes input received from any keyboard-like input system. Accepts a 
	void ProcessRoll(float offset);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset);
};
#endif