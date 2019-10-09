#pragma once
#define GUI_H
#ifdef GUI_H

#include "glad/glad.h"
#include "nanogui/nanogui.h"
#include "Fractal.h"

class Fractal;
class Form;

class GUI : public nanogui::Screen
{
public:
	Fractal* fractal;
	Form* form;
	nanogui::Window* nanoGuiWindow;
	GUI(GLFWwindow* mainWindow, Fractal* fractal);
};

class Form : public nanogui::FormHelper
{
public:
	GUI* gui;
	Form(GUI* gui);
	template<typename T>
	void AddSlider(std::string label, T& value);
};

#endif

template<typename T>
inline void Form::AddSlider(std::string label, T& value)
{
	nanogui::Label* labelW = new nanogui::Label(gui, label, mLabelFontName, 20);
	nanogui::Slider* slider = new nanogui::Slider(gui->nanoGuiWindow);
	slider->setValue(0.5f);

	nanogui::Vector2i fs = slider->fixedSize();
	slider->setFixedSize(nanogui::Vector2i(fs.x() != 0 ? fs.x() : mFixedSize.x(),
		fs.y() != 0 ? fs.y() : mFixedSize.y()));

	if (mLayout->rowCount() > 0)
		mLayout->appendRow(mVariableSpacing);
	mLayout->appendRow(0);
	mLayout->setAnchor(labelW, nanogui::AdvancedGridLayout::Anchor(1, mLayout->rowCount() - 1));
	mLayout->setAnchor(slider, nanogui::AdvancedGridLayout::Anchor(3, mLayout->rowCount() - 1));

}
