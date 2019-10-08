#pragma once
#define GUI_H
#ifdef GUI_H

#include "glad/glad.h"
#include "nanogui/nanogui.h"
#include "Fractal.h"

class Fractal;

class GUI : public nanogui::Screen
{
public:
	Fractal* fractal;
	nanogui::FormHelper* form;
	GUI(GLFWwindow* mainWindow, Fractal* fractal);
};

#endif