#pragma once
#define GUI_H
#ifdef GUI_H

#include <glad/glad.h>
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

	static const constexpr char* guiWindowName = "Fractal variables";
};



class Form : public nanogui::FormHelper
{
public:
	GUI* gui;
	Form(GUI* gui);
	//template<typename T>
	//void AddSlider(std::string label, T& value);

	template <typename Type> nanogui::Slider*
		AddSlider(const std::string& label, const std::function<void(const Type&)>& setter,
			const std::function<Type()>& getter) {
		nanogui::Label* labelW = new nanogui::Label(mWindow, label, mLabelFontName, mLabelFontSize);
		nanogui::Slider* widget = new nanogui::Slider(gui->nanoGuiWindow);
		auto refresh = [widget, getter] {
			Type value = getter(), current = widget->value();
			if (value != current)
				widget->setValue(value);
		};
		refresh();
		widget->setCallback(setter);
		widget->setFontSize(mWidgetFontSize);
		nanogui::Vector2i fs = widget->fixedSize();
		widget->setFixedSize(nanogui::Vector2i(fs.x() != 0 ? fs.x() : mFixedSize.x(),
			fs.y() != 0 ? fs.y() : mFixedSize.y()));
		mRefreshCallbacks.push_back(refresh);
		if (mLayout->rowCount() > 0)
			mLayout->appendRow(mVariableSpacing);
		mLayout->appendRow(0);
		mLayout->setAnchor(labelW, nanogui::AdvancedGridLayout::Anchor(1, mLayout->rowCount() - 1));
		mLayout->setAnchor(widget, nanogui::AdvancedGridLayout::Anchor(3, mLayout->rowCount() - 1));
		return widget;
	}

	template <typename Type> nanogui::Slider*
		AddSlider(const std::string& label, Type& value, bool editable = true) {
		return AddSlider<Type>(label,
			[&](const Type& v) { value = v; },
			[&]() -> Type { return value; }
			);
	}
};

#endif

//template<typename T>
//inline void Form::AddSlider(std::string label, T& value)
//{
//	nanogui::Label* labelW = new nanogui::Label(gui, label, mLabelFontName, 20);
//	nanogui::Slider* slider = new nanogui::Slider(gui->nanoGuiWindow);
//	slider->setValue(0.5f);
//
//	nanogui::Vector2i fs = slider->fixedSize();
//	slider->setFixedSize(nanogui::Vector2i(fs.x() != 0 ? fs.x() : mFixedSize.x(),
//		fs.y() != 0 ? fs.y() : mFixedSize.y()));
//
//	if (mLayout->rowCount() > 0)
//		mLayout->appendRow(mVariableSpacing);
//	mLayout->appendRow(0);
//	mLayout->setAnchor(labelW, nanogui::AdvancedGridLayout::Anchor(1, mLayout->rowCount() - 1));
//	mLayout->setAnchor(slider, nanogui::AdvancedGridLayout::Anchor(3, mLayout->rowCount() - 1));
//
//}

