#pragma once
#define GUI_H
#ifdef GUI_H

#include <glad/glad.h>
#include "headers/Fractal.h"
#include "nanogui/nanogui.h"
#include "Fractal.h"
#include "headers/Uniform.h"
#include <string>

class Fractal;
class Form;

class GuiElement
{
public:
	enum class Element
	{
		Slider,
		TextBox,
		error
	};
	Fractal* fractal;
	void* uniform;

	GuiElement(Element element, std::string type, std::string uniformName, std::string elementLabel, Fractal* fractal, std::string value, std::vector<std::string> guiParams);
	static Element GetElementFromString(std::string element);

private:
	void* CreateUniform(std::string type, std::string name, std::string value);
};

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
	template <typename Type> nanogui::Slider* AddSlider(const std::string& label, const std::function<void(const Type&)>& setter, const std::function<Type()>& getter)
	{
		nanogui::Label* labelW = new nanogui::Label(mWindow, label, mLabelFontName, mLabelFontSize);
		nanogui::Slider* widget = new nanogui::Slider(gui->nanoGuiWindow);
		auto refresh = [widget, getter] {
			Type value = (Type)getter(), current = widget->value();
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

	template <typename Type> nanogui::Slider* AddSlider(const std::string& label, Type& value, bool editable = true)
	{
		return AddSlider<Type>(label,
			[&](const Type& v) { value = v; },
			[&]() -> Type { return value; }
			);
	}

	nanogui::CheckBox* AddCheckbox(const std::string& label, const std::function<void(const bool&)>& setter, const std::function<bool()>& getter)
	{
		nanogui::Label* labelW = new nanogui::Label(mWindow, label, mLabelFontName, mLabelFontSize);
		nanogui::CheckBox* widget = new nanogui::CheckBox(gui->nanoGuiWindow, "");
		auto refresh = [widget, getter] {
			bool value = getter(), current = widget->pushed();
			if (value != current)
				widget->setPushed(value);
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

	nanogui::CheckBox* AddCheckbox(const std::string& label, bool& value, bool editable = true)
	{
		return AddCheckbox(label,
			[&](const bool& v) { value = v; },
			[&]() -> bool { return value; }
			);
	}
};

#endif
