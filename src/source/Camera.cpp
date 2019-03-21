#include "headers/Camera.h"
#include <glm.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

Camera::Camera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f), float Yaw = YAW, float Pitch = PITCH) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), movementReverse(1)
{
	position = Position;
	yaw = Yaw;
	pitch = Pitch;
}

Camera::Camera(float posX, float posY, float posZ, float Yaw, float Pitch) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), movementReverse(1)
{
	position = glm::vec3(posX, posY, posZ);
	yaw = Yaw;
	pitch = Pitch;
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

glm::vec3 Camera::GetForwardVector()
{
	glm::vec3 out;

	float rads2 = static_cast<float>(abs(fmod(glm::radians(pitch), M_PI)));
	float percentNotLookingMiddle = static_cast<float>((rads2 < M_PI_2) ?
		rads2 / M_PI_2 :
		(M_PI - rads2) / M_PI_2);

	// Vertical movement
	out.y = cos(glm::radians(pitch) + static_cast<float>(M_PI_2)) * movementReverse * -1 * percentNotLookingMiddle;


	// Horizontal movement
	glm::vec2 verticalFront(1 * movementReverse, 0);

	verticalFront = GetYawMatrix2() * verticalFront;

	// If we face up, we wanna move slower horizontally
	float percentLookingMiddle = static_cast<float>(abs(rads2 - M_PI_2) / M_PI_2);

	out += glm::vec3(verticalFront.x, 0, verticalFront.y) * percentLookingMiddle;

	return glm::normalize(out);
}


glm::vec3 Camera::GetRightVector()
{
	return glm::normalize(glm::cross(GetForwardVector(), worldUp));
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;

	if (direction == forward)
		position += GetForwardVector() * velocity;
	if (direction == back)
		position -= GetForwardVector() * velocity;

	if (direction == right)
		position += GetRightVector() * velocity;
	if (direction == left)
		position -= GetRightVector() * velocity;

	if (direction == up)
		position += worldUp * velocity;
	if (direction == down)
		position -= worldUp * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;
}