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
	nanogui::Slider* slider = fractal->gui->form->AddSlider(label, uniform->value);
	slider->setCallback([fractal,uniform](T value)
		{
			uniform->value = value;
			fractal->explorationShader->SetUniform(*uniform);
		});
	slider->setRange({ range.first,range.second });
	slider->setValue(value);
	uniform->SetGuiValue = [slider, uniform]() {slider->setValue(uniform->GetValue()); };
}

void AddColorPicker(std::string label, Uniform<nanogui::Color>* uniform, Fractal* fractal)
{
	nanogui::ColorPicker* picker = fractal->gui->form->AddColorPicker(label, ((Uniform<nanogui::Color>*)uniform)->value);
	uniform->SetGuiValue = [picker, uniform]() {picker->setColor(uniform->GetValue()); 	picker->setBackgroundColor(uniform->GetValue()); };
	picker->setCallback([fractal, uniform](const nanogui::Color& c)
		{
			uniform->GetValue() = c;
			fractal->explorationShader->SetUniform(*uniform);
			uniform->SetGuiValue();
		});
}

std::string GuiElement::GetElement(std::vector<std::string>& content, std::string name)
{
	for (size_t i = 0; i < content.size(); i++)
	{
		size_t start;
		if ((start = content[i].find(name)) != std::string::npos)
		{
			size_t colon = content[i].find(':', start);
			if (colon == std::string::npos) continue;

			colon++;

			return content[i].substr(colon);
		}
	}
	return "";
}

GuiElement::GuiElement(Element element, std::string type, std::string uniformName, std::string elementLabel, Fractal* fractal, std::string value, std::vector<std::string> guiParams) : fractal(fractal)
{
	uniform = CreateUniform(type, uniformName, value, element);
	
	void* uniformCopy = uniform;

	if (element == Element::Slider)
	{
		std::string range = GetElement(guiParams, "Range");
		Fractal::CleanString(range, { '(',')',' ' });

		std::vector<std::string> rangeV = Fractal::Split(range, ',');
		std::pair<float, float> rangePair = { stof(rangeV[0]),stof(rangeV[1]) };


		if (type == "float")
		{
			AddSlider<float>(elementLabel, (Uniform<float>*)uniform, fractal, rangePair, stof(value));
			SetGuiValue = [uniformCopy]() {((Uniform<float>*)uniformCopy)->SetGuiValue(); };
		}
		else if (type == "int")
		{
			AddSlider<int>(elementLabel, (Uniform<int>*)uniform, fractal, rangePair, stoi(value));
			SetGuiValue = [uniformCopy]() {((Uniform<int>*)uniformCopy)->SetGuiValue(); };
		}
		else
		{
			std::cout << "Support for type " << type << " has not been added yet" << std::endl;
			BreakIfDebug();
		}
	}
	else if (element == Element::ColorPicker)
	{
		AddColorPicker(elementLabel, (Uniform<nanogui::Color>*)uniform, fractal);
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
	else if (iequals("colorPicker", element))
	{
		return Element::ColorPicker;
	}
	else
	{
		return Element::error;
	}
}

void* GuiElement::CreateUniform(std::string type, std::string name, std::string value, Element elementType)
{
	fractal->explorationShader->use();
	unsigned int id = glGetUniformLocation(fractal->explorationShader->id, name.c_str());
	if (type == "float")
	{
		return new Uniform<float>((value == "") ? 0.f : std::stof(value), name, id);
	}
	else if (type == "int")
	{
		return new Uniform<int>((value == "") ? 0.f : std::stoi(value), name, id);
	}
	else if (type == "vec3")
	{
		if (value.substr(0,4)=="vec3")
		{
			value = value.substr(4);
		}
		
		Fractal::CleanString(value, { '(',')' });

		std::vector<std::string> values = Fractal::Split(value, ',');
		if (elementType == Element::ColorPicker)
		{
			return new Uniform<nanogui::Color>(nanogui::Color(stof(values[0]),stof(values[1]),stof(values[2]), 1.f), name, id);
		}
		else
		{
			return new Uniform<glm::vec3>(glm::vec3(stof(values[0]), stof(values[1]), stof(values[2])), name, id);
		}
	}
	else
	{
		std::cout << "Fatal error: unknown type for uniform " << name << std::endl;
		BreakIfDebug();
		return nullptr;
	}
}
