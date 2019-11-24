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
			uniform->SetValue(value, Fractal::renderMode);
			fractal->shader->SetUniform(*uniform);
		});
	uniform->SetShaderValue = ([uniform, fractal](bool renderMode)
		{
			fractal->shader->SetUniform(*uniform, renderMode);
		});
	slider->setRange({ range.first,range.second });
	slider->setValue((float)value);
	uniform->SetGuiValue = [slider, uniform]() {slider->setValue((float)uniform->GetValue()); };
}

void AddCheckBox(std::string label, Uniform<bool>* uniform, Fractal* fractal, bool value)
{
	nanogui::CheckBox* slider = fractal->gui->form->AddCheckbox(label, uniform->value);
	slider->setCallback([fractal,uniform](bool value)
		{
			uniform->SetValue(value, Fractal::renderMode);
			fractal->shader->SetUniform(*uniform);
		});
	uniform->SetShaderValue = ([uniform, fractal](bool renderMode)
		{
			fractal->shader->SetUniform(*uniform, renderMode);
		});
	slider->setChecked(value);
	uniform->SetGuiValue = [slider, uniform]() {slider->setChecked(uniform->GetValue()); };
}

void AddColorPicker(std::string label, Uniform<nanogui::Color>* uniform, Fractal* fractal)
{
	nanogui::ColorPicker* picker = fractal->gui->form->AddColorPicker(label, ((Uniform<nanogui::Color>*)uniform)->value);
	uniform->SetGuiValue = [picker, uniform]() {picker->setColor(uniform->GetValue()); 	picker->setBackgroundColor(uniform->GetValue()); };
	picker->setCallback([fractal, uniform](const nanogui::Color& c)
		{
			uniform->SetValue(c, Fractal::renderMode);
			fractal->shader->SetUniform(*uniform);
			uniform->SetGuiValue();
		});
	uniform->SetShaderValue = ([uniform, fractal](bool renderMode)
		{
			fractal->shader->SetUniform(*uniform, renderMode);
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
	std::string renderValue;
	renderValue = GuiElement::GetElement(guiParams, "RenderValue");


	uniform = CreateUniform(type, uniformName, value, renderValue, element);
	
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
			SetShaderValue = [uniformCopy](bool renderMode) {((Uniform<float>*)uniformCopy)->SetShaderValue(renderMode); };
		}
		else if (type == "int")
		{
			AddSlider<int>(elementLabel, (Uniform<int>*)uniform, fractal, rangePair, stoi(value));
			SetGuiValue = [uniformCopy]() {((Uniform<int>*)uniformCopy)->SetGuiValue(); };
			SetShaderValue = [uniformCopy](bool renderMode) {((Uniform<int>*)uniformCopy)->SetShaderValue(renderMode); };
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
		SetGuiValue = [uniformCopy]() {((Uniform<nanogui::Color>*)uniformCopy)->SetGuiValue(); };
		SetShaderValue = [uniformCopy](bool renderMode) {((Uniform<nanogui::Color>*)uniformCopy)->SetShaderValue(renderMode); };
	}
	else if (element == Element::CheckBox)
	{
		AddCheckBox(elementLabel, (Uniform<bool>*)uniform, fractal, value != "false");
		SetGuiValue = [uniformCopy]() {((Uniform<bool>*)uniformCopy)->SetGuiValue(); };
		SetShaderValue = [uniformCopy](bool renderMode) {((Uniform<bool>*)uniformCopy)->SetShaderValue(renderMode); };
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
	else if (iequals("checkBox", element))
	{
		return Element::CheckBox;
	}
	else
	{
		return Element::error;
	}
}

void* GuiElement::CreateUniform(std::string type, std::string name, std::string value, std::string renderValue, Element elementType)
{
	fractal->shader->Use();
	unsigned int id = glGetUniformLocation(fractal->shader->id, name.c_str());
	if (type == "float")
	{
		float val = (value == "") ? 0.f : std::stof(value);
		float renderVal = (renderValue == "") ? val : std::stof(renderValue);
		return new Uniform<float>(val, renderVal, name, id);
	}
	else if (type == "int")
	{
		int val = (value == "") ? 0 : std::stoi(value);
		int renderVal = (renderValue == "") ? val : std::stoi(renderValue);
		return new Uniform<int>(val, renderVal, name, id);
	}
	else if (type == "bool")
	{
		bool val = (value != "false") ? true : false;
		bool renderVal = (renderValue == "") ? val : ((renderValue != "false") ? true : false);
		return new Uniform<bool>(val, renderVal, name, id);
	}
	else if (type == "vec3")
	{
		if (value.substr(0,4)=="vec3")
		{
			value = value.substr(4);
		}
		
		Fractal::CleanString(value, { '(',')' });

		std::vector<std::string> values = Fractal::Split(value, ',');
		std::vector<std::string> renderValues = (renderValue == "") ? values : Fractal::Split(renderValue, ',');
		if (elementType == Element::ColorPicker)
		{
			return new Uniform<nanogui::Color>(nanogui::Color(stof(values[0]),stof(values[1]),stof(values[2]), 1.f),
											   nanogui::Color(stof(renderValues[0]), stof(renderValues[1]), stof(renderValues[2]), 1.f), name, id);
		}
		else
		{
			return new Uniform<glm::vec3>(glm::vec3(stof(values[0]), stof(values[1]), stof(values[2])),
										  glm::vec3(stof(renderValues[0]), stof(renderValues[1]), stof(renderValues[2])), name, id);
		}
	}
	else
	{
		std::cout << "Fatal error: unknown type for uniform " << name << std::endl;
		BreakIfDebug();
		return nullptr;
	}
}
