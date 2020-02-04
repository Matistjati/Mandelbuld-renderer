#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include "headers/Fractal.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class CameraMovement
{
	forward,
	back,
	left,
	right,
	up,
	down
};

class Fractal;

// Default camera
#define DefaultCamera3D new Camera(glm::vec3(1.8f, 0.8f, -0.6f), /*Position*/ 169, -14, /*Yaw, pitch*/ 0.15f, 3, 0.5, 1 /*mouseSensitivity, movementSpeed, scrollSpeed, zoom*/, true /* viewMode3D*/)
#define DefaultCamera2D new Camera(glm::vec3(0, 0, 0), /*Position*/ 0, 0, /*Yaw, pitch*/ 0, 1, 0.5, 1 /*mouseSensitivity, movementSpeed, scrollSpeed, zoom*/, false /* viewMode3D*/)

// A camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:

	bool viewMode3D;

	// Camera Attributes
	Uniform<glm::vec3> position;
	Uniform<float> zoom;

	// Viewing direction
	float GetYaw();
	float GetPitch();
	void SetYaw(float v);
	void SetPitch(float v);
	Uniform<glm::mat3>& GetRotationMatrix();

	// Camera options
	float movementSpeed;
	float mouseSensitivity;
	float scrollSpeed;

	Camera(const glm::vec3 position, float yaw, float pitch, float mouseSensitivity, float movementSpeed, float scrollSpeed, float zoom, bool viewMode3D = true);

	glm::vec3 GetWorldUp();

	glm::vec3 GetWorldForward();

	// For moving forward or backward
	glm::vec3 GetForwardVector();

	// For moving left or right
	glm::vec3 GetRightVector();

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessMovement(CameraMovement direction, float magnitude);

	void ProcessZoom(float magnitude);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(glm::vec2 offset);
private:
	// Euler Angles
	float yaw;
	float pitch;
	bool rotationMatrixIsCurrent;
	Uniform<glm::mat3> rotation;

	void SetRotationMatrix();
};
#endif