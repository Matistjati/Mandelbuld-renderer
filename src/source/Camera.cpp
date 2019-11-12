#include "headers/Camera.h"
#include <glm.hpp>

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

float Camera::GetRoll()
{
	return roll;
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

void Camera::SetRoll(float v)
{
	roll = v;
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

Camera::Camera(const glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f), float Yaw = YAW, float Pitch = PITCH, float Roll = ROLL) : movementSpeed(SPEED), mouseSensitivity(SENSITIVITY),
	rollSpeed(ROLLSPEED), worldFlip(-1), rotationMatrixIsCurrent(false)
{
	position.value = Position;
	yaw = Yaw;
	pitch = Pitch;
	roll = Roll;
}

Camera::Camera(const glm::vec3 Position, float Yaw, float Pitch, float Roll, float MouseSensitivity, float MovementSpeed, float RollSpeed) : worldFlip(-1), rotationMatrixIsCurrent(false)
{
	position.value = Position;
	yaw = Yaw;
	pitch = Pitch;
	roll = Roll;
	mouseSensitivity = MouseSensitivity;
	movementSpeed = MovementSpeed;
	rollSpeed = RollSpeed;
}

Camera::Camera(float posX, float posY, float posZ, float Yaw, float Pitch, float Roll = ROLL) : movementSpeed(SPEED), mouseSensitivity(SENSITIVITY),
	rollSpeed(ROLLSPEED), worldFlip(-1), rotationMatrixIsCurrent(false)
{
	position.value = glm::vec3(posX, posY, posZ);
	yaw = Yaw;
	pitch = Pitch;
	roll = Roll;
}

void Camera::SetRotationMatrix()
{
	// The standard 3d rotation matrices
	float p = glm::radians(-GetPitch());
	glm::mat3 pitchMatrix = glm::mat3(1,     0,       0,
									  0, cos(p), -sin(p),
									  0, sin(p),  cos(p));


	float y = glm::radians(GetYaw());
	glm::mat3 yawMatrix = glm::mat3(cos(y), 0, sin(y),
								    0,	    1,      0,
								   -sin(y), 0, cos(y));


	float r = glm::radians(-GetRoll());
	glm::mat3 rollMatrix = glm::mat3(cos(r), -sin(r),  0,
									 sin(r),  cos(r),  0,
									 0,       0,       1);

	rotation.value = pitchMatrix * yawMatrix * rollMatrix;
}


// Upwards vector independent of roll
glm::vec3 Camera::GetWorldUp()
{
	glm::vec2 result = glm::vec2(0, 1) * cos(glm::radians(roll)) + glm::vec2(1, 0) * sin(glm::radians(roll));
	return glm::normalize(glm::vec3(0, result.y, -result.x));
}

// Forward vector independent of roll
glm::vec3 Camera::GetWorldForward()
{
	return glm::normalize(glm::vec3(abs(cos(glm::radians(roll))), 0, 0));
}


glm::vec3 Camera::GetForwardVector()
{
	glm::vec3 out;
	// Vertical movement
	out.y = -cos(glm::radians(pitch) + static_cast<float>(M_PI_2));


	glm::vec3 forward = rotation.value * glm::vec3(1, 0, 0);
	forward.y = 0;
	float rads = static_cast<float>(abs(fmod(glm::radians(pitch), M_PI) - M_PI_2)); // Move less horizontally when looking down
	forward *= sin(rads);
	out += forward;
	
	// Zero if very small
	const static float epsilon = 0.00001f;
	out.x = (abs(out.x) < epsilon) ? 0 : out.x;
	out.y = (abs(out.y) < epsilon) ? 0 : out.y;

	return glm::normalize(out);
}


glm::vec3 Camera::GetRightVector()
{
	return glm::normalize(glm::cross(GetForwardVector(), worldUp));
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

// Give a positive or negative float representing the change you want to see
void Camera::ProcessRoll(float delta)
{
	SetRoll(roll + delta);
}

void Camera::ProcessMouseMovement(glm::vec2 offset)
{
	SetYaw(yaw + offset.x);

	const static float pitchLimit = 89.999f;
	// Lock horizontal movement (making an "impossible" turn breaks movement)
	SetPitch(glm::clamp(GetPitch() + offset.y, -pitchLimit, pitchLimit));
}
