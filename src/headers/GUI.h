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
	const std::string guiWindowName = "Fractal variables";

	GUI(GLFWwindow* mainWindow, Fractal* fractal);
	void ClearFocusPath();
};



class Form : public nanogui::FormHelper
{
public:
	GUI* gui;
	Form(GUI* gui);

	// C++ doesnt let me define templated methods in another file
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