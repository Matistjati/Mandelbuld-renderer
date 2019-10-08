#include "headers/GUI.h"

GUI::GUI(GLFWwindow* mainWindow, Fractal* fractal) : nanogui::Screen(), fractal(fractal)
{
	this->initialize(mainWindow, true);

	// Create nanogui gui
	bool enabled = true;
	form = new nanogui::FormHelper(this);
	nanogui::ref<nanogui::Window> nanoguiWindow = form->addWindow(Eigen::Vector2i(10, 10), "Fractal variables");

	this->setVisible(true);
} 
