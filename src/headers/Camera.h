#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include "headers/Fractal.h"

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
	Uniform<int> worldFlip;

	// Camera Attributes
	Uniform<glm::vec3> position;

	// Matrices
	Uniform<glm::mat2> yawMatrix;
	Uniform<glm::mat2> pitchMatrix;
	Uniform<glm::mat2> rollMatrix;

	float GetYaw();
	float GetRoll();
	float GetPitch();
	void SetYaw(float v);
	void SetRoll(float v);
	void SetPitch(float v);

	// Camera options
	float movementSpeed;
	float mouseSensitivity;
	float rollSpeed;

	// Used for orientation
	const glm::vec3 worldUp = glm::vec3(0, 1, 0);
	
	// Constructor with vectors
	Camera(const glm::vec3 Position, float Yaw, float Pitch, float Roll);
	Camera(const glm::vec3 Position, float Yaw, float Pitch, float Roll, float mouseSensitivity, float movementSpeed, float rollSpeed);

	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float Yaw, float Pitch, float roll);


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
private:
	// Euler Angles
	float yaw;
	float pitch;
	float roll;

	// Rotation around the x axis
	void SetYawMatrix();

	// Rotation around the y axis
	void SetPitchMatrix();

	// Rotation around the z axis
	void SetRollMatrix();
};
#endif