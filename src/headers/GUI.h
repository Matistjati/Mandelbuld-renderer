#pragma once
#define GUI_H
#ifdef GUI_H

#include <glad/glad.h>
#include "headers/Fractal.h"
#include "nanogui/nanogui.h"
#include "Fractal.h"
#include "headers/Uniform.h"
#include "glm.hpp"
#include <string>

class Fractal;
class Form;
class SubMenu;

enum class Element
{
	Slider,
	TextBox,
	ColorPicker,
	CheckBox,
	Button,
	SubMenu,
	error
};

class GuiElement
{
public:
	Fractal* fractal;
	void* uniform;
	Element element;

	static std::string GetElement(std::vector<std::string>& content, std::string name);
	GuiElement(Element element, std::string type, std::string uniformName, std::string elementLabel, Fractal* fractal, std::string value, std::string parent, std::vector<std::string> guiParams);
	GuiElement(Element element, void* uniform, Fractal* fractal);
	GuiElement();

	void DeleteUniform() { delete uniform; }

	static Element GetElementFromString(std::string element);

private:
	void* CreateUniform(std::string type, std::string name, std::string value, std::string renderValue, Element elementType);
};

class SubMenu
{
public:
	Element element;
	std::string name;
	std::string identifier;
	Fractal* fractal;
	std::vector<nanogui::Widget*> children;

	nanogui::Window* window;
	Form* form;

	SubMenu(Element element, std::string name, std::string identifier, Fractal* fractal);

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
	Form(GUI* gui, nanogui::Button* button, SubMenu* parentMenu);
	nanogui::Button* parentButton;
	SubMenu* parentMenu;

	// C++ doesnt let me define templated methods in another file
	template <typename Type> nanogui::Slider* AddSlider(nanogui::Window* window, const std::string& label, const std::function<void(const Type&)>& setter, const std::function<Type()>& getter)
	{
		nanogui::Label* labelW = new nanogui::Label(mWindow, label, mLabelFontName, mLabelFontSize);
		nanogui::Slider* widget = new nanogui::Slider(window);

		auto refresh = [widget, getter] {
			Type value = (Type)getter(), current = (Type)widget->value();
			if (value != current)
				widget->setValue((float)value);
		};
		refresh();

		widget->setFontSize(mWidgetFontSize);
		Eigen::Vector2i fs = widget->fixedSize();

		widget->setFixedSize(Eigen::Vector2i(fs.x() != 0 ? fs.x() : mFixedSize.x(),
			fs.y() != 0 ? fs.y() : mFixedSize.y()));

		mRefreshCallbacks.push_back(refresh);
		if (mLayout->rowCount() > 0)
			mLayout->appendRow(mVariableSpacing);
		mLayout->appendRow(0);
		mLayout->setAnchor(labelW, nanogui::AdvancedGridLayout::Anchor(1, mLayout->rowCount() - 1));
		mLayout->setAnchor(widget, nanogui::AdvancedGridLayout::Anchor(2, mLayout->rowCount() - 1));
		return widget;
	}

	template <typename Type> nanogui::Slider* AddSlider(const std::string& label, const std::function<void(const Type&)>& setter, const std::function<Type()>& getter)
	{
		return AddSlider<Type>(gui->nanoGuiWindow, label, setter, getter);
	}

	template <typename Type> nanogui::Slider* AddSlider(nanogui::Window* window, const std::string& label, Type& value)
	{
		return AddSlider<Type>(window, label,
			[&](const Type& v) { value = v; },
			[&]() -> Type { return value; }
			);
	}

	template <typename Type> nanogui::Slider* AddSlider(const std::string& label, Type& value)
	{
		return AddSlider<Type>(label,
			[&](const Type& v) { value = v; },
			[&]() -> Type { return value; }
			);
	}

	std::vector<nanogui::Slider*> AddSlidersN(nanogui::Window* window, const std::function<glm::vec4()>& getter, int n, std::pair<float, float> range, glm::vec4 value)
	{
		std::vector<nanogui::Slider*> sliders(n);
		const char dimensionNames[] = { 'X', 'Y', 'Z', 'W' };
		for (int i = 0; i < n; i++)
		{
			nanogui::Label* labelW = new nanogui::Label(mWindow, std::string(1,dimensionNames[i]), mLabelFontName, mLabelFontSize);
			nanogui::Slider* widget = new nanogui::Slider(window);

			auto refresh = [widget, getter, i] {
				float value = getter()[i], current = widget->value();
				if (value != current)
					widget->setValue(value);
			};
			refresh();

			widget->setRange(range);
			widget->setValue(value[i]);

			widget->setFontSize(mWidgetFontSize);
			Eigen::Vector2i fs = widget->fixedSize();

			widget->setFixedSize(Eigen::Vector2i(fs.x() != 0 ? fs.x() : mFixedSize.x(),
				fs.y() != 0 ? fs.y() : mFixedSize.y()));

			mRefreshCallbacks.push_back(refresh);
			if (mLayout->rowCount() > 0)
				mLayout->appendRow(mVariableSpacing);
			mLayout->appendRow(0);
			mLayout->setAnchor(labelW, nanogui::AdvancedGridLayout::Anchor(1, mLayout->rowCount() - 1));
			mLayout->setAnchor(widget, nanogui::AdvancedGridLayout::Anchor(2, mLayout->rowCount() - 1));
			sliders[i] = widget;
		}
		
		return sliders;
	}

	nanogui::Button* AddButton(nanogui::Window* parent, const std::string& label, std::string caption)
	{
		nanogui::Label* labelW = new nanogui::Label(mWindow, label, mLabelFontName, mLabelFontSize);
		nanogui::Button* widget = new nanogui::Button(parent);

		widget->setCaption(caption);

		widget->setFontSize(mWidgetFontSize);
		nanogui::Vector2i fs = widget->fixedSize();
		widget->setFixedSize(Eigen::Vector2i(fs.x() != 0 ? fs.x() : mFixedSize.x(),
			fs.y() != 0 ? fs.y() : mFixedSize.y()));

		if (mLayout->rowCount() > 0)
			mLayout->appendRow(mVariableSpacing);

		mLayout->appendRow(0);
		mLayout->setAnchor(labelW, nanogui::AdvancedGridLayout::Anchor(1, mLayout->rowCount() - 1));
		mLayout->setAnchor(widget, nanogui::AdvancedGridLayout::Anchor(2, mLayout->rowCount() - 1));
		return widget;
	}

	nanogui::Button* AddButton(const std::string& label, std::string caption)
	{
		return AddButton(gui->nanoGuiWindow, label, caption);
	}

	nanogui::Button* AddButton(nanogui::Window* window, const std::string& label)
	{
		return AddButton(window, label, "open");
	}

	nanogui::Button* AddButton(const std::string& label)
	{
		return AddButton(gui->nanoGuiWindow, label, "open");
	}

	nanogui::CheckBox* AddCheckbox(nanogui::Window* window, const std::string& label, const std::function<void(const bool&)>& setter, const std::function<bool()>& getter)
	{
		nanogui::Label* labelW = new nanogui::Label(mWindow, label, mLabelFontName, mLabelFontSize);
		nanogui::CheckBox* widget = new nanogui::CheckBox(window, "");
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
		mLayout->setAnchor(widget, nanogui::AdvancedGridLayout::Anchor(2, mLayout->rowCount() - 1));
		return widget;
	}

	nanogui::CheckBox* AddCheckbox(const std::string& label, const std::function<void(const bool&)>& setter, const std::function<bool()>& getter)
	{
		return AddCheckbox(gui->nanoGuiWindow, label,
			setter,
			getter);
	}

	nanogui::CheckBox* AddCheckbox(nanogui::Window* window, const std::string& label, bool& value)
	{
		return AddCheckbox(window, label,
			[&](const bool& v) { value = v; },
			[&]() -> bool { return value; }
			);
	}

	nanogui::CheckBox* AddCheckbox(const std::string& label, bool& value)
	{
		return AddCheckbox(gui->nanoGuiWindow, label,
			[&](const bool& v) { value = v; },
			[&]() -> bool { return value; }
			);
	}
	
	nanogui::ColorPicker* AddColorPicker(nanogui::Window* window, const std::string& label, const std::function<void(const nanogui::Color&)>& setter, const std::function<nanogui::Color()>& getter)
	{
		nanogui::Label* labelW = new nanogui::Label(mWindow, label, mLabelFontName, mLabelFontSize);
		nanogui::ColorPicker* widget = new nanogui::ColorPicker(window);
		auto refresh = [widget, getter] {
			nanogui::Color value = getter(), current = widget->color();
			if (value != current)
				widget->setColor(value);
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
		mLayout->setAnchor(widget, nanogui::AdvancedGridLayout::Anchor(2, mLayout->rowCount() - 1));
		return widget;
	}

	nanogui::ColorPicker* AddColorPicker(const std::string& label, const std::function<void(const nanogui::Color&)>& setter, const std::function<nanogui::Color()>& getter)
	{
		return AddColorPicker(gui->nanoGuiWindow, label, setter, getter);
	}


	nanogui::ColorPicker* AddColorPicker(nanogui::Window* window, const std::string& label, nanogui::Color& value)
	{
		return AddColorPicker(window, label,
			[&](const nanogui::Color& v) { value = v; },
			[&]() -> nanogui::Color { return value; }
			);
	}

	nanogui::ColorPicker* AddColorPicker(const std::string& label, nanogui::Color& value)
	{
		return AddColorPicker(gui->nanoGuiWindow, label,
			[&](const nanogui::Color& v) { value = v; },
			[&]() -> nanogui::Color { return value; }
			);
	}

	template <typename Type> nanogui::detail::FormWidget<Type>*
		AddTextBox(const std::string& label, const std::function<void(const Type&)>& setter,
			const std::function<Type()>& getter, bool editable = true) {
		nanogui::Label* labelW = new nanogui::Label(mWindow, label, mLabelFontName, mLabelFontSize);
		auto widget = new nanogui::detail::FormWidget<Type>(mWindow);
		auto refresh = [widget, getter] {
			Type value = getter(), current = widget->value();
			if (value != current)
				widget->setValue(value);
		};
		refresh();
		widget->setCallback(setter);
		widget->setEditable(editable);
		widget->setFontSize(mWidgetFontSize);
		nanogui::Vector2i fs = widget->fixedSize();
		widget->setFixedSize(nanogui::Vector2i(fs.x() != 0 ? fs.x() : mFixedSize.x(),
			fs.y() != 0 ? fs.y() : mFixedSize.y()));
		mRefreshCallbacks.push_back(refresh);
		if (mLayout->rowCount() > 0)
			mLayout->appendRow(mVariableSpacing);
		mLayout->appendRow(0);
		mLayout->setAnchor(labelW, nanogui::AdvancedGridLayout::Anchor(1, mLayout->rowCount() - 1));
		mLayout->setAnchor(widget, nanogui::AdvancedGridLayout::Anchor(2, mLayout->rowCount() - 1));
		return widget;
	}

	/// Add a new data widget that exposes a raw variable in memory
	template <typename Type> nanogui::detail::FormWidget<Type>*
		AddTextBox(const std::string& label, Type& value, bool editable = true) {
		return AddTextBox<Type>(label,
			[&](const Type& v) { value = v; },
			[&]() -> Type { return value; },
			editable
			);
	}
};

#endif
