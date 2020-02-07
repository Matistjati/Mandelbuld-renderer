#include "headers/Camera.h"
#include <glm.hpp>
#include "gtc/matrix_transform.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <string>
#include "headers/ToString.h"
#include "headers/Debug.h"

void Camera::PopulateCameraGUI(Fractal* fractal)
{
	// 3D option
	Uniform<bool>* checkBox3D = new Uniform<bool>(viewMode3D);

	nanogui::CheckBox* view3D = cameraMenu->form->AddCheckbox(cameraMenu->window, "3D", checkBox3D, fractal, checkBox3D->value);

	view3D->setCallback([this, fractal, checkBox3D](bool mode)
		{
			this->viewMode3D = mode;

			nanogui::Window* window = this->cameraMenu->window;

			this->cameraMenu->window = this->cameraMenu->form->addWindow(this->cameraMenu->window->position(), "Camera");

			this->PopulateCameraGUI(fractal);
			fractal->gui->performLayout();
			delete checkBox3D;
			window->dispose();
		});

	// Zoom
	GuiElement zoomElement = GuiElement();
	zoomElement.element = Element::TextBox;
	zoomElement.fractal = fractal;
	zoomElement.uniform = &fractal->camera->zoom;
	fractal->fractalUniforms.push_back(zoomElement);

	auto zoomField = cameraMenu->form->AddTextBox("Zoom", fractal->camera->zoom.value);
	zoomField->setCallback([fractal](float value)
		{
			fractal->camera->zoom.SetValue(value, Fractal::renderMode);
			fractal->shader->SetUniform(fractal->camera->zoom);
		});

	fractal->camera->zoom.guiElements = { zoomField };
	fractal->camera->zoom.SetGuiValue = [fractal]() { ((nanogui::detail::FormWidget<float, std::true_type>*)fractal->camera->zoom.guiElements[0])->setValue(fractal->camera->zoom.value); };
	fractal->camera->zoom.SetShaderValue = [fractal](bool renderMode) { fractal->shader->SetUniform(fractal->camera->zoom); };

	// Position
	GuiElement positionElement = GuiElement();
	positionElement.element = Element::TextBox;
	positionElement.fractal = fractal;
	positionElement.uniform = &fractal->camera->position;
	fractal->fractalUniforms.push_back(positionElement);

	
	cameraMenu->form->addGroup("Position");

	auto positionFieldX = cameraMenu->form->AddTextBox("X", position.value.x);
	positionFieldX->setCallback([fractal](float value)
		{
			fractal->camera->position.SetValue({ value, fractal->camera->position.value.y, fractal->camera->position.value.z }, false);
			fractal->shader->SetUniform(fractal->camera->position);
		});

	positionFieldX->numberFormat("%.6g");

	auto positionFieldY = cameraMenu->form->AddTextBox("Y", position.value.y);
	positionFieldY->setCallback([fractal](float value)
		{
			fractal->camera->position.SetValue({ fractal->camera->position.value.x, value, fractal->camera->position.value.z }, false);
			fractal->shader->SetUniform(fractal->camera->position);
		});

	positionFieldY->numberFormat("%.6g");

	position.guiElements = { positionFieldX, positionFieldY };
	position.SetGuiValue = [this]() {
		for (size_t i = 0; i < this->position.guiElements.size(); i++)
		{
			((nanogui::detail::FormWidget<float, std::true_type>*)this->position.guiElements[i])->setValue(this->position.value[i]);
		}
	};
	position.SetShaderValue = [fractal](bool renderMode) {fractal->shader->SetUniform(fractal->camera->position, false); };

	if (viewMode3D)
	{
		auto positionFieldZ = cameraMenu->form->AddTextBox("Z", position.value.y);
		positionFieldY->setCallback([fractal](float value)
			{
				fractal->camera->position.SetValue({ fractal->camera->position.value.x, fractal->camera->position.value.y, value }, false);
				fractal->shader->SetUniform(fractal->camera->position);
			});

		positionFieldY->numberFormat("%.6g");
		position.guiElements.push_back(positionFieldZ);
	}

	// Rotation
	GuiElement rotationElement = GuiElement();
	rotationElement.element = Element::error;
	rotationElement.fractal = fractal;
	rotationElement.uniform = &rotation;
	fractal->fractalUniforms.push_back(rotationElement);
}

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

Camera::Camera(const glm::vec3 position, float yaw, float pitch, float mouseSensitivity, float movementSpeed, float scrollSpeed, float zoom, bool viewMode3D) : position(position),
	yaw(yaw), pitch(pitch),	mouseSensitivity(mouseSensitivity), movementSpeed(movementSpeed), scrollSpeed(scrollSpeed), rotationMatrixIsCurrent(false), viewMode3D(viewMode3D),
	zoom(zoom), cameraMenu(), cursorVisible(!viewMode3D), mouseOffset(), firstMouse(true), lastNonGuiPos(), useBuddhabrotRotation(false)
{ }

void Camera::SetRotationMatrix(glm::vec3 offset)
{
	// The standard 3d rotation matrices: https://en.wikipedia.org/wiki/Rotation_matrix#In_three_dimensions

	if (!useBuddhabrotRotation)
	{
		float p = glm::radians(GetPitch());
		glm::mat3 pitch = glm::mat3(1, 0, 0,
			0, cos(p), -sin(p),
			0, sin(p), cos(p));

		float y = glm::radians(-GetYaw());
		glm::mat3 yaw = glm::mat3(cos(y), 0, sin(y),
			0, 1, 0,
			-sin(y), 0, cos(y));

		rotation.SetValue(pitch * yaw, false);
	}
	else
	{
		float p = glm::radians(GetPitch());
		float y = glm::radians(GetYaw());
		glm::vec3 s = { sin(p)+offset.y, sin(y)+offset.x, sin(0) };
		glm::vec3 c = { cos(p)+offset.y, cos(y)+offset.x, cos(0) };
		glm::mat3 rot;
		rot[0] = glm::vec3(c.y * c.z, c.y * s.z, -s.y);
		rot[1] = glm::vec3(s.x * s.y * c.z - c.x * s.z, s.x * s.y * s.z + c.x * c.z, s.x * c.y);
		rot[2] = glm::vec3(c.x * s.y * c.z + s.x * s.z, c.x * s.y * s.z - s.x * c.z, c.x * c.y);
		rotation.SetValue(rot, false);
	}	
}


glm::vec3 Camera::GetWorldUp()
{
	if (useBuddhabrotRotation) return glm::vec3(0, -1, 0);
	else					   return glm::vec3(0,  1, 0);
}

glm::vec3 Camera::GetWorldForward()
{
	return glm::vec3(0, 0, -1);
}


glm::vec3 Camera::GetForwardVector()
{
	glm::vec3 out;
	// Vertical movement
	out = GetWorldUp() * cos(glm::radians(-pitch) + static_cast<float>(M_PI_2));

	if (useBuddhabrotRotation)
	{
		glm::vec3 forward = glm::vec3(1, 0, 0);

		forward.y = 0;
		float rads = static_cast<float>(abs(fmod(glm::radians(pitch), M_PI) - M_PI_2)); // Move less horizontally when looking down
		forward *= sin(rads);
		out += forward;
	}
	else
	{
		float yaw = glm::radians(GetYaw()) + static_cast<float>(M_PI_2);
		glm::vec3 forward = glm::vec3(-cos(yaw), 0, -sin(yaw));
		forward.y = 0;
		float rads = static_cast<float>(abs(fmod(glm::radians(pitch), M_PI) - M_PI_2)); // Move less horizontally when looking down
		forward *= sin(rads);
		out += forward;
	}
	
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

	glm::vec3 startValue = position.value;

	switch (direction)
	{
	case CameraMovement::forward:
		if (viewMode3D)	position.value += GetForwardVector() * velocity;
		else position.value.y += velocity;
		break;
	case CameraMovement::back:
		if (viewMode3D)	position.value -= GetForwardVector() * velocity;
		else position.value.y -= velocity;
		break;
	case CameraMovement::right:
		if (viewMode3D)	position.value += GetRightVector() * velocity;
		else position.value.x += velocity;
		break;
	case CameraMovement::left:
		if (viewMode3D)	position.value -= GetRightVector() * velocity;
		else position.value.x -= velocity;
		break;
	case CameraMovement::up:
		if (viewMode3D) position.value += GetWorldUp() * velocity;
		break;
	case CameraMovement::down:
		if (viewMode3D) position.value -= GetWorldUp() * velocity;
		break;
	default:
		break;
	}

	// Call callbacks if position changed
	if (position.value != startValue)
	{
		position.SetValue(position.value, false);
		position.SetGuiValue();
	}
}

void Camera::ProcessZoom(float magnitude)
{
	zoom.SetValue(zoom.value * magnitude, false);
	if (viewMode3D)
	{
		zoom.SetValue(glm::clamp(zoom.GetValue(), 1e-7f, 1.f), false);
	}
}

void Camera::ProcessMouseMovement(glm::vec2 offset)
{
	SetYaw(yaw + offset.x);
	//std::cout << (fmod(glm::radians(yaw), M_PI*2)) << std::endl;

	const static float pitchLimit = 89.999f;
	// Lock horizontal movement (making an "impossible" turn breaks movement)
	SetPitch(glm::clamp(GetPitch() + offset.y, -pitchLimit, pitchLimit));
}
