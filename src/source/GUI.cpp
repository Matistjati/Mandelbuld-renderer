#include "headers/GUI.h"

GUI::GUI(GLFWwindow* mainWindow, Fractal* fractal) : nanogui::Screen(), fractal(fractal)
{
	using namespace nanogui;

	this->initialize(mainWindow, true);

	// Create nanogui gui
	bool enabled = true;
	FormHelper* gui = new FormHelper(this);
	ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");
	gui->addGroup("Basic types");

	gui->addGroup("Validating fields");

	auto zoomButton = gui->addVariable("Zoom", fractal->zoom.value);
	zoomButton->setSpinnable(true);
	zoomButton->setCallback([this](float value) {
		std::cout << "dab";
		this->fractal->zoom.value = value;
		this->fractal->explorationShader->SetUniform(this->fractal->zoom);
		});




	gui->addGroup("Other widgets");
	gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; })->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;



	this->setVisible(true);
	this->performLayout();
	nanoguiWindow->center();
	//nanoguiWindow->setVisible(false);

	performLayout();
} 
