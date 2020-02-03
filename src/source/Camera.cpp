#include "headers/Camera.h"
#include <glm.hpp>
#include "gtc/matrix_transform.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <string>
#include "headers/ToString.h"
#include "headers/Debug.h"

float Camera::GetYaw()
{
	return yaw;
}

float Camera::GetPitch()
{
	return pitch;
}

void Camera::SetYaw(float v)
{
	yaw = v;
	rotationMatrixIsCurrent = false;
}

void Camera::SetPitch(float v)
{
	pitch = v;
	rotationMatrixIsCurrent = false;
}

Uniform<glm::mat3>& Camera::GetRotationMatrix()
{
	if (!rotationMatrixIsCurrent)
	{
		SetRotationMatrix();
		rotationMatrixIsCurrent = true;
	}
	return rotation;
}

Camera::Camera(const glm::vec3 position, float yaw, float pitch, float mouseSensitivity, float movementSpeed, float scrollSpeed) : position(position), yaw(yaw), pitch(pitch),
	mouseSensitivity(mouseSensitivity), movementSpeed(movementSpeed), scrollSpeed(scrollSpeed), rotationMatrixIsCurrent(false)
{ }

void Camera::SetRotationMatrix()
{
	// The standard 3d rotation matrices
	float p = glm::radians(GetPitch());
	glm::mat3 pitch = glm::mat3(1, 0, 0, 0, cos(p), -sin(p), 0, sin(p), cos(p));
	
	float y = glm::radians(-GetYaw());
	glm::mat3 yaw = glm::mat3(cos(y), 0, sin(y), 0, 1, 0, -sin(y), 0, cos(y));

	rotation.value = pitch * yaw;
}


glm::vec3 Camera::GetWorldUp()
{
	return glm::vec3(0, 1, 0);
}

glm::vec3 Camera::GetWorldForward()
{
	return glm::vec3(0, 0, -1);
}


glm::vec3 Camera::GetForwardVector()
{
	glm::vec3 out;
	// Vertical movement
	out.y = -cos(glm::radians(pitch) + static_cast<float>(M_PI_2));


	glm::vec3 forward = glm::vec3(-cos(glm::radians(GetYaw()) + M_PI_2), 0, -sin(glm::radians(GetYaw()) + M_PI_2));
	forward.y = 0;
	float rads = static_cast<float>(abs(fmod(glm::radians(pitch), M_PI) - M_PI_2)); // Move less horizontally when looking down
	forward *= sin(rads);
	out += forward;
	
	// Zero if very small
	const static float epsilon = 0.0001f;
	out.y = (abs(out.y) < epsilon) ? 0 : out.y;

	return glm::normalize(out);
}


glm::vec3 Camera::GetRightVector()
{
	return glm::normalize(glm::cross(GetForwardVector(), GetWorldUp()));
}

void Camera::ProcessMovement(CameraMovement direction, float magnitude)
{
	float velocity = movementSpeed * magnitude;

	switch (direction)
	{
	case CameraMovement::forward:
		position.value += GetForwardVector() * velocity;
		break;
	case CameraMovement::back:
		position.value -= GetForwardVector() * velocity;
		break;
	case CameraMovement::right:
		position.value += GetRightVector() * velocity;
		break;
	case CameraMovement::left:
		position.value -= GetRightVector() * velocity;
		break;
	case CameraMovement::up:
		position.value += GetWorldUp() * velocity;
		break;
	case CameraMovement::down:
		position.value -= GetWorldUp() * velocity;
		break;
	default:
		break;
	}
}

void Camera::ProcessMouseMovement(glm::vec2 offset)
{
	SetYaw(yaw + offset.x);

	const static float pitchLimit = 89.999f;
	// Lock horizontal movement (making an "impossible" turn breaks movement)
	SetPitch(glm::clamp(GetPitch() + offset.y, -pitchLimit, pitchLimit));
}
