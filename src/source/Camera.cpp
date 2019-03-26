#include "headers/Camera.h"
#include <glm.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <string>
#include "headers/ToString.h"

Camera::Camera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f), float Yaw = YAW, float Pitch = PITCH, float Roll = ROLL) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), movementReverse(1), rollSpeed(ROLLSPEED)
{
	position = Position;
	yaw = Yaw;
	pitch = Pitch;
	roll = Roll;
}

Camera::Camera(float posX, float posY, float posZ, float Yaw, float Pitch, float Roll = ROLL) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), movementReverse(1), rollSpeed(ROLLSPEED)
{
	position = glm::vec3(posX, posY, posZ);
	yaw = Yaw;
	pitch = Pitch;
	roll = Roll;
}

glm::mat2 Camera::GetYawMatrix2()
{
	float rad = glm::radians(-yaw);
	return glm::mat2(cos(rad), -sin(rad),
					 sin(rad), cos(rad));
}

glm::mat2 Camera::GetPitchMatrix2()
{
	float rad = glm::radians(pitch);
	return glm::mat2(cos(rad), -sin(rad),
					 sin(rad), cos(rad));
}

glm::mat2 Camera::GetRollMatrix2()
{
	float rad = glm::radians(roll);
	return glm::mat2(cos(rad), -sin(rad),
					 sin(rad), cos(rad));
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
	return glm::normalize(glm::vec3(-abs(cos(glm::radians(roll))) * -movementReverse, 0, 0));
}


glm::vec3 Camera::GetForwardVector()
{
	glm::vec3 out;

	float rads = static_cast<float>(abs(fmod(glm::radians(pitch), M_PI)));
	float percentNotLookingMiddle = static_cast<float>((rads < M_PI_2) ?
		rads / M_PI_2 :
		(M_PI - rads) / M_PI_2);

	//std::cout << std::to_string(glm::radians(roll)) << std::endl;
	// Vertical movement
	glm::vec3 up = GetWorldUp();
	out += up * cos(glm::radians(pitch) + static_cast<float>(M_PI_2)) * -movementReverse;// *percentNotLookingMiddle;// -sin(glm::radians(roll));

	// Horizontal movement
	glm::vec2 verticalFront = GetWorldForward();

	verticalFront = GetYawMatrix2() * verticalFront;// *GetRollMatrix2();

	// If we face up, we wanna move slower horizontally
	float percentLookingMiddle = static_cast<float>(abs(rads - M_PI_2) / M_PI_2);

	out += glm::vec3(verticalFront.x, 0, verticalFront.y) * percentLookingMiddle;

#ifdef _DEBUG
	std::cout << ToString::toString(out) << std::endl;
#endif

	return glm::normalize(out);
}


glm::vec3 Camera::GetRightVector()
{
	return glm::normalize(glm::cross(GetForwardVector(), worldUp));
}

void Camera::ProcessMovement(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;

	if (direction == forward)
		position += GetForwardVector() * velocity;
	if (direction == back)
		position -= GetWorldForward() * velocity;

	if (direction == right)
		position += GetRightVector() * velocity;
	if (direction == left)
		position -= GetRightVector() * velocity;

	if (direction == up)
		position += GetWorldUp() * velocity;
	if (direction == down)
		position -= GetWorldUp() * velocity;
}

void Camera::ProcessRoll(float offset)
{
	roll = offset;

#if _DEBUG
	std::cout << "cos: " << std::to_string(cos(glm::radians(roll))) << std::endl;
#endif
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	//float rads = static_cast<float>(abs(fmod(glm::radians(pitch), M_PI)));
	//float percentLookingMiddle = static_cast<float>(abs(rads - M_PI_2) / M_PI_2);

	//std::cout << std::to_string(percentLookingMiddle) << std::endl;
}