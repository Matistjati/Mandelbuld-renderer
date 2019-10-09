#include "headers/GUI.h"

GUI::GUI(GLFWwindow* mainWindow, Fractal* fractal) : nanogui::Screen(), fractal(fractal)
{
	this->initialize(mainWindow, true);

	// Create nanogui gui
	form = new Form(this);
	nanoGuiWindow = form->addWindow(Eigen::Vector2i(10, 10), "Fractal variables");

	this->setVisible(true);
} 

Form::Form(GUI* gui) : nanogui::FormHelper(gui), gui(gui)
{

}
