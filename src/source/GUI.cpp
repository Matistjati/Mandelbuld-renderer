#include "headers/GUI.h"
#include "headers/Debug.h"

GUI::GUI(GLFWwindow* mainWindow, Fractal* fractal) : nanogui::Screen(), fractal(fractal)
{
	this->initialize(mainWindow, true);

	// Create nanogui gui
	form = new Form(this);
	nanoGuiWindow = form->addWindow(Eigen::Vector2i(10, 10), guiWindowName);

	this->setVisible(true);
}

void GUI::ClearFocusPath()
{
	mFocusPath.clear();
}


Form::Form(GUI* gui) : nanogui::FormHelper(gui), gui(gui)
{

}

template<typename T>
void AddSlider(std::string label, Uniform<T>* uniform, Fractal* fractal, std::pair<float, float> range, T value)
{
	auto slider = fractal->gui->form->AddSlider(label, uniform->value);
	slider->setCallback([fractal,uniform](T value)
		{
			uniform->value = value;
			fractal->explorationShader->SetUniform(*uniform);
		});
	slider->setRange({ range.first,range.second });
	slider->setValue(value);
}

GuiElement::GuiElement(Element element, std::string type, std::string uniformName, std::string elementLabel, Fractal* fractal, std::string value, std::vector<std::string> guiParams) : fractal(fractal)
{
	uniform = CreateUniform(type, uniformName, value);
	
	if (element == Element::Slider)
	{
		if (type == "float")
		{
			AddSlider<float>(elementLabel, (Uniform<float>*)uniform, fractal, { stof(guiParams[2]), stof(guiParams[3]) }, stof(value));
		}
		else if (type == "int")
		{
			AddSlider<int>(elementLabel, (Uniform<int>*)uniform, fractal, { stoi(guiParams[2]), stoi(guiParams[3]) }, stoi(value));
		}
		else
		{
			std::cout << "Support for type " << type << " has not been added yet" << std::endl;
		}
	}


	else if (element == Element::error)
	{
		std::cout << "Could not determine GUI element type" << std::endl;
		BreakIfDebug();
	}
}

bool iequals(const std::string& a, const std::string& b)
{
	return std::equal(a.begin(), a.end(),
		b.begin(), b.end(),
		[](char a, char b) {
			return tolower(a) == tolower(b);
		});
}

GuiElement::Element GuiElement::GetElementFromString(std::string element)
{
	if (iequals("Slider", element))
	{
		return Element::Slider;
	}
	else if (iequals("TextBox", element))
	{
		return Element::TextBox;
	}
	else
	{
		return Element::error;
	}
}

void* GuiElement::CreateUniform(std::string type, std::string name, std::string value)
{
	fractal->explorationShader->use();
	if (type == "float")
	{
		return new Uniform<float>((value == "") ? 0.f : std::stof(value), name, glGetUniformLocation(fractal->explorationShader->id, name.c_str()));
	}
	else if (type == "int")
	{
		return new Uniform<int>((value == "") ? 0.f : std::stoi(value), name, glGetUniformLocation(fractal->explorationShader->id, name.c_str()));
	}
	else
	{
		std::cout << "Fatal error: unknown type for uniform " << name << std::endl;
		BreakIfDebug();
		return nullptr;
	}
}
