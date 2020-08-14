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


Form::Form(GUI* gui) : nanogui::FormHelper(gui), gui(gui), parentButton(), parentMenu()
{

}

Form::Form(GUI* gui, nanogui::Button* button, SubMenu* parentMenu) : nanogui::FormHelper(gui), gui(gui), parentButton(button), parentMenu(parentMenu)
{

}

template<typename T>
void AddSlider(Form* form, nanogui::Window* window, std::string label, Uniform<T>* uniform, Fractal* fractal, std::pair<float, float> range, T value)
{
	nanogui::Slider* slider = form->AddSlider(window, label, uniform->value);
	slider->setCallback([fractal,uniform](T value)
		{
			uniform->SetValue(value, Fractal::renderMode);
			fractal->shader->SetUniform(*uniform);
			fractal->shader->Use();
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

template<typename T>
void AddSlidersN(Form* form, nanogui::Window* parent, std::string label, int n, Uniform<T>* uniform, Fractal* fractal, std::pair<float, float> range, T value)
{
	nanogui::Button* button = form->AddButton(parent, label);
	
	//fractal->gui->performLayout();


	Form* subForm = new Form(fractal->gui, button, form->parentMenu);

	nanogui::Window* window = subForm->addWindow(form->parentButton->position() + Eigen::Vector2i(form->parentButton->size().x(), (int)(form->parentButton->size().y() / 2.5))
		+ button->position() + Eigen::Vector2i(button->size().x(), (int)(button->size().y() / 2)) + Eigen::Vector2i{ 50,0 }, label);

	form->parentMenu->children.push_back(window);

	window->setVisible(false);

	glm::vec4 initValue = { -1,-1,-1,-1 };
	for (int i = 0; i < T::length(); i++)
	{
		initValue[i] = value[i];
	}

	std::vector<nanogui::Slider*> sliders = subForm->AddSlidersN(window, [uniform]() -> glm::vec4
		{	
			glm::vec4 returnV = { -1,-1,-1,-1 };
			for (int i = 0; i < T::length(); i++)
			{
				returnV[i] = uniform->GetValue()[i];
			}
			return returnV; 
		},
		n, range, initValue);

	for (int i = 0; i < n; i++)
	{
		sliders[i]->setCallback([i,fractal, uniform](float value)
			{
				T val = uniform->GetValue();
				val[i] = value;
				uniform->SetValue(val, Fractal::renderMode);
				fractal->shader->SetUniform(*uniform);
			});
	}


	button->setCallback([fractal,uniform,window]()
		{
			window->setVisible(!window->visible());
		});
	uniform->SetShaderValue = ([uniform, fractal](bool renderMode)
		{
			fractal->shader->SetUniform(*uniform, renderMode);
		});

	uniform->SetGuiValue = [sliders, uniform]() { for (size_t i = 0; i < sliders.size(); i++) { sliders[i]->setValue(uniform->GetValue()[i]); } };

	uniform->guiElements = { };
	for (size_t i = 0; i < sliders.size(); i++)
	{
		uniform->guiElements.push_back(sliders[i]);
	}
}

nanogui::CheckBox* Form::AddCheckbox(nanogui::Window* parent, std::string label, Uniform<bool>* uniform, Fractal* fractal, bool value)
{
	nanogui::CheckBox* checkBox = AddCheckbox(parent, label, uniform->value);
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

	return checkBox;
}

class ColorWrapper : public nanogui::ColorPicker
{
public:
	// Nanogui won't let us change the color when the user is "editing" (mPushed) it, even though it requests it being changed. Oh well
	void SetColorTrue(nanogui::Color col)
	{
		bool state = mPushed;
		mPushed = false;
		setColor(col);
		mPushed = state;
	}
};

void AddColorPicker(Form* form, nanogui::Window* parent, std::string label, Uniform<nanogui::Color>* uniform, Fractal* fractal)
{
	nanogui::ColorPicker* picker = form->AddColorPicker(parent, label, ((Uniform<nanogui::Color>*)uniform)->value);
	uniform->SetGuiValue = [picker, fractal, uniform]()
	{
		if (!fractal->holdingMouse)
		{
			((ColorWrapper*)picker)->SetColorTrue(uniform->GetValue());
		}
		picker->setBackgroundColor(uniform->GetValue());
	};
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

GuiElement::GuiElement(Element element, std::string type, std::string uniformName, std::string elementLabel, Fractal* fractal, std::string value, std::string parent, std::vector<std::string> guiParams) : fractal(fractal)
{
	this->element = element;
	std::string renderValue;
	renderValue = GuiElement::GetElement(guiParams, "RenderValue");


	uniform = CreateUniform(type, uniformName, value, renderValue, element);
	
	nanogui::Window* window = nullptr;
	Form* form = nullptr;
	if (parent != "")
	{
		for (size_t i = 0; i < fractal->subMenus.size(); i++)
		{
			if (Fractal::StringEqualNoCase(parent, fractal->subMenus[i]->identifier))
			{
				window = fractal->subMenus[i]->window;
				form = fractal->subMenus[i]->form;
				break;
			}
		}

		if (!window || !form)
		{
			window = fractal->gui->nanoGuiWindow;
			form = fractal->gui->form;
			DebugPrint("Could not find correct parent");
			BreakIfDebug();
		}
	}
	else
	{
		form = fractal->gui->form;
		window = fractal->gui->nanoGuiWindow;
	}
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
			AddSlider<float>(form, window, elementLabel, (Uniform<float>*)uniform, fractal, rangePair, stof(value));
		}
		else if (type == "int")
		{
			AddSlider<int>(form, window, elementLabel, (Uniform<int>*)uniform, fractal, rangePair, stoi(value));
		}
		else if (type == "vec2")
		{
			if (value.substr(0, 4) == "vec2")
			{
				value = value.substr(4);
			}

			Fractal::CleanString(value, { '(',')' });

			std::vector<std::string> values = Fractal::Split(value, ',');

			if (values.size() == 1)
			{
				values.push_back(values[0]);
			}

			AddSlidersN(form, window, elementLabel, 2, (Uniform<glm::vec2>*)uniform, fractal, rangePair, glm::vec2(stof(values[0]), stof(values[1])));
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

			AddSlidersN(form, window, elementLabel, 3, (Uniform<glm::vec3>*)uniform, fractal, rangePair, glm::vec3(stof(values[0]), stof(values[1]), stof(values[2])));
		}
		else if (type == "vec4")
		{
			if (value.substr(0, 4) == "vec4")
			{
				value = value.substr(4);
			}

			Fractal::CleanString(value, { '(',')' });

			std::vector<std::string> values = Fractal::Split(value, ',');

			if (values.size() == 1)
			{
				values.push_back(values[0]);
				values.push_back(values[0]);
				values.push_back(values[0]);
			}

			AddSlidersN(form, window, elementLabel, 4, (Uniform<glm::vec4>*)uniform, fractal, rangePair, glm::vec4(stof(values[0]), stof(values[1]), stof(values[2]), stof(values[3])));
		}
		else
		{
			std::cout << "Support for type " << type << " has not been added yet" << std::endl;
			BreakIfDebug();
		}
	}
	else if (element == Element::ColorPicker)
	{
		AddColorPicker(form, window, elementLabel, (Uniform<nanogui::Color>*)uniform, fractal);
	}
	else if (element == Element::CheckBox)
	{
		form->AddCheckbox(window, elementLabel, (Uniform<bool>*)uniform, fractal, value != "false");
	}
	else if (element == Element::error)
	{
		std::cout << "Could not determine GUI element type" << std::endl;
		BreakIfDebug();
	}
}

GuiElement::GuiElement(Element element, UniformSuper* uniform, Fractal* fractal) : element(element), uniform(uniform), fractal(fractal) { }

SubMenu::SubMenu(Element element, std::string name, std::string identifier, Fractal* fractal, Eigen::Vector2i offset, SubMenu* parent) : element(element), name(name), fractal(fractal), identifier(identifier), children(), window(), form()
{
	if (element == Element::SubMenu)
	{
		Form* parentForm = (parent == nullptr) ? fractal->gui->form : parent->form;

		nanogui::Button* button = parentForm->AddButton(name);

		fractal->gui->performLayout();


		form = new Form(fractal->gui, button, this);
		Eigen::Vector2i parentOffset = (parent == nullptr) ? (Eigen::Vector2i{30, 0}) : parent->form->parentMenu->window->position();
		window = form->addWindow(parentOffset + button->position() + Eigen::Vector2i(button->size().x(), (int)(button->size().y()/2)) + offset, name);

		form->parentButton->setCallback([this]()
			{
				window->setVisible(!window->visible());
				if (!window->visible())
				{
					for (size_t i = 0; i < form->parentMenu->children.size(); i++)
					{
						form->parentMenu->children[i]->setVisible(false);
					}
				}
			});

		window->setVisible(false);
	}
	else
	{
		std::cout << "Could not determine GUI element type" << std::endl;
		BreakIfDebug();
	}
}
//
//SubMenu::SubMenu(Element element, std::string name, std::string identifier, Fractal* fractal, SubMenu* parent, Eigen::Vector2i offset) : element(element), name(name), fractal(fractal), identifier(identifier), children(), window(), form()
//{
//	if (element == Element::SubMenu)
//	{
//		nanogui::Button* button = parent->form->AddButton(name);
//
//		fractal->gui->performLayout();
//
//
//		form = new Form(fractal->gui, button, this);
//		window = form->addWindow(parent->form->parentMenu->window->position() + button->position() + Eigen::Vector2i(button->size().x(), (int)(button->size().y() / 2)) + Eigen::Vector2i{ 30,0 } + offset, name);
//
//		form->parentButton->setCallback([this]()
//			{
//				window->setVisible(!window->visible());
//				if (!window->visible())
//				{
//					for (size_t i = 0; i < form->parentMenu->children.size(); i++)
//					{
//						form->parentMenu->children[i]->setVisible(false);
//					}
//				}
//			});
//
//		window->setVisible(false);
//	}
//	else
//	{
//		std::cout << "Could not determine GUI element type" << std::endl;
//		BreakIfDebug();
//	}
//}

GuiElement::GuiElement() : element(Element::error), fractal(), uniform()
{
}


Element GuiElement::GetElementFromString(std::string element)
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
	else if (Fractal::StringEqualNoCase("submenu", element))
	{
		return Element::SubMenu;
	}
	else
	{
		return Element::error;
	}
}

UniformSuper* GuiElement::CreateUniform(std::string type, std::string name, std::string value, std::string renderValue, Element elementType)
{
	const static bool isObjectMember = false;
	fractal->shader->Use();
	unsigned int programId = fractal->shader->id;
	unsigned int id = glGetUniformLocation(fractal->shader->id, name.c_str());
	if (id == GL_INVALID_INDEX)
	{
		if (fractal->shader->type == ShaderType::compute)
		{
			((ComputeShader*)fractal->shader)->UseRender();
			id = glGetUniformLocation(((ComputeShader*)fractal->shader)->renderId, name.c_str());
			if (id != GL_INVALID_INDEX)
			{
				programId = ((ComputeShader*)fractal->shader)->renderId;
			}
		}
	}

	if (type == "float")
	{
		float val = (value == "") ? 0.f : std::stof(value);
		float renderVal = (renderValue == "") ? val : std::stof(renderValue);
		return new Uniform<float>(val, renderVal, name, id, programId, isObjectMember);
	}
	else if (type == "int")
	{
		int val = (value == "") ? 0 : std::stoi(value);
		int renderVal = (renderValue == "") ? val : std::stoi(renderValue);
		return new Uniform<int>(val, renderVal, name, id, programId, isObjectMember);
	}
	else if (type == "bool")
	{
		bool val = (value != "false") ? true : false;
		bool renderVal = (renderValue == "") ? val : ((renderValue != "false") ? true : false);
		return new Uniform<bool>(val, renderVal, name, id, programId, isObjectMember);
	}
	else if (type == "vec2")
	{
		if (value.substr(0, 4) == "vec2")
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


		return new Uniform<glm::vec2>(glm::vec2(stof(values[0]), stof(values[1])),
			glm::vec2(stof(renderValues[0]), stof(renderValues[1])), name, id, programId, isObjectMember);
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
#pragma warning(push)
#pragma warning(disable: 4316)
			return new Uniform<nanogui::Color>(nanogui::Color(stof(values[0]),stof(values[1]),stof(values[2]), 1.f),
											   nanogui::Color(stof(renderValues[0]), stof(renderValues[1]), stof(renderValues[2]), 1.f), name, id, programId, isObjectMember);
#pragma warning(pop)
		}
		else
		{
			return new Uniform<glm::vec3>(glm::vec3(stof(values[0]), stof(values[1]), stof(values[2])),
										  glm::vec3(stof(renderValues[0]), stof(renderValues[1]), stof(renderValues[2])), name, id, programId, isObjectMember);
		}
	}
	else if (type == "vec4")
	{
		if (value.substr(0, 4) == "vec4")
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
			values.push_back(values[0]);
		}
		if (renderValues.size() == 1)
		{
			renderValues.push_back(renderValues[0]);
			renderValues.push_back(renderValues[0]);
			renderValues.push_back(renderValues[0]);
		}


		return new Uniform<glm::vec4>(glm::vec4(stof(values[0]), stof(values[1]), stof(values[2]), stof(values[3])),
			glm::vec4(stof(renderValues[0]), stof(renderValues[1]), stof(renderValues[2]), stof(renderValues[3])), name, id, programId, isObjectMember);
	}
	else
	{
		std::cout << "Fatal error: unknown type for uniform " << name << std::endl;
		BreakIfDebug();
		return nullptr;
	}
}
