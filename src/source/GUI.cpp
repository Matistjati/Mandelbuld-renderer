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
	uniform->guiElements = { slider };
}

void AddSlider3(std::string label, Uniform<glm::vec3>* uniform, Fractal* fractal, std::pair<float, float> range, glm::vec3 value)
{
	nanogui::Button* button = fractal->gui->form->AddButton(label);

	fractal->gui->performLayout();


	Form* form = new Form(fractal->gui);
	nanogui::Window* window = form->addWindow(fractal->gui->form->GetBottomElementPos() + Eigen::Vector2i{110,0}, label);

	window->setVisible(false);

	std::vector<nanogui::Slider*> sliders = form->AddSlidersN(window, [uniform]() -> glm::vec4 {return glm::vec4(uniform->GetValue(), std::numeric_limits<double>::quiet_NaN()); },
		3, range, glm::vec4(value.x, value.y, value.z, -1));
	
	sliders[0]->setCallback([fractal, uniform](float value)
		{
			uniform->SetValue(glm::vec3(value, uniform->value.y, uniform->value.z), Fractal::renderMode);
			fractal->shader->SetUniform(*uniform);
		});

	sliders[1]->setCallback([fractal, uniform](float value)
		{
			uniform->SetValue(glm::vec3(uniform->value.x, value, uniform->value.z), Fractal::renderMode);
			fractal->shader->SetUniform(*uniform);
		});

	sliders[2]->setCallback([fractal, uniform](float value)
		{
			uniform->SetValue(glm::vec3(uniform->value.x, uniform->value.y, value), Fractal::renderMode);
			fractal->shader->SetUniform(*uniform);
		});


	button->setCallback([fractal,uniform,window]()
		{
			window->setVisible(!window->visible());
		});
	uniform->SetShaderValue = ([uniform, fractal](bool renderMode)
		{
			fractal->shader->SetUniform(*uniform, renderMode);
		});

	uniform->SetGuiValue = [sliders, uniform]() { sliders[0]->setValue(uniform->GetValue().x); sliders[1]->setValue(uniform->GetValue().y); sliders[2]->setValue(uniform->GetValue().z); };

	uniform->guiElements = { sliders[0], sliders[1], sliders[2] };
}

void AddCheckBox(std::string label, Uniform<bool>* uniform, Fractal* fractal, bool value)
{
	nanogui::CheckBox* checkBox = fractal->gui->form->AddCheckbox(label, uniform->value);
	checkBox->setCallback([fractal,uniform](bool value)
		{
			uniform->SetValue(value, Fractal::renderMode);
			fractal->shader->SetUniform(*uniform);
		});
	uniform->SetShaderValue = ([uniform, fractal](bool renderMode)
		{
			fractal->shader->SetUniform(*uniform, renderMode);
		});
	checkBox->setChecked(value);
	uniform->SetGuiValue = [checkBox, uniform]() {checkBox->setChecked(uniform->GetValue()); };

	uniform->guiElements = { checkBox };
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

	uniform->guiElements = { picker };
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
	this->element = element;
	std::string renderValue;
	renderValue = GuiElement::GetElement(guiParams, "RenderValue");


	uniform = CreateUniform(type, uniformName, value, renderValue, element);
	
	void* uniformCopy = uniform;

	if (element == Element::Slider)
	{
		std::string range = GetElement(guiParams, "Range");
		Fractal::CleanString(range, { '(',')',' ' });

		std::vector<std::string> rangeV = Fractal::Split(range, ',');
		if (!rangeV.size())
		{
			DebugPrint("Range not declared for " + elementLabel);
			BreakIfDebug();
		}
		std::pair<float, float> rangePair = { stof(rangeV[0]),stof(rangeV[1]) };


		if (type == "float")
		{
			AddSlider<float>(elementLabel, (Uniform<float>*)uniform, fractal, rangePair, stof(value));
		}
		else if (type == "int")
		{
			AddSlider<int>(elementLabel, (Uniform<int>*)uniform, fractal, rangePair, stoi(value));
		}
		else if (type == "vec3")
		{
			if (value.substr(0, 4) == "vec3")
			{
				value = value.substr(4);
			}

			Fractal::CleanString(value, { '(',')' });

			std::vector<std::string> values = Fractal::Split(value, ',');

			if (values.size() == 1)
			{
				values.push_back(values[0]);
				values.push_back(values[0]);
			}

			AddSlider3(elementLabel, (Uniform<glm::vec3>*)uniform, fractal, rangePair, glm::vec3(stof(values[0]), stof(values[1]), stof(values[2])));
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
	else if (element == Element::CheckBox)
	{
		AddCheckBox(elementLabel, (Uniform<bool>*)uniform, fractal, value != "false");
	}
	else if (element == Element::error)
	{
		std::cout << "Could not determine GUI element type" << std::endl;
		BreakIfDebug();
	}
}

GuiElement::GuiElement(Element element, void* uniform, Fractal* fractal) : element(element), uniform(uniform), fractal(fractal) { }


GuiElement::Element GuiElement::GetElementFromString(std::string element)
{
	if (Fractal::StringEqualNoCase("Slider", element))
	{
		return Element::Slider;
	}
	else if (Fractal::StringEqualNoCase("TextBox", element))
	{
		return Element::TextBox;
	}
	else if (Fractal::StringEqualNoCase("colorPicker", element))
	{
		return Element::ColorPicker;
	}
	else if (Fractal::StringEqualNoCase("checkBox", element))
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

		if (values.size() == 1)
		{
			values.push_back(values[0]);
			values.push_back(values[0]);
		}
		if (renderValues.size() == 1)
		{
			renderValues.push_back(renderValues[0]);
			renderValues.push_back(renderValues[0]);
		}

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
