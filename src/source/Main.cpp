#include <windows.h>
#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <algorithm>

#include "headers/Camera.h"
#include "headers/shader.h"
#include "headers/Image.h"
#include "headers/FileManager.h"
#include "headers/Fractal3D.h"
#include "headers/Fractal2D.h"
#include "headers/Debug.h"

enum Purpose
{
	singleImage,
	explore,
};

constexpr Purpose programPurpose = Purpose::explore;

// Screensize
#define ConstWindowSize 1
#if ConstWindowSize
	#define ScreenSize 2
	
	// Common screen resolutions
	#if ScreenSize == 0
		const glm::ivec2 screenSize = { 500, 500 };
	#elif ScreenSize == 1
		const glm::ivec2 screenSize = { 1280, 720 };
	#elif ScreenSize == 2
		const glm::ivec2 screenSize = { 1920, 1080 };
	#elif ScreenSize == 3
		const glm::ivec2 screenSize = { 2560, 1440 };
	#elif ScreenSize == 4
		const glm::ivec2 screenSize = { 3840, 2160 };
	#elif ScreenSize == 5
		const glm::ivec2 screenSize = { 7680, 4320 };
	#endif
#endif

// Starting fractal
#define DefaultFractal Fractal3D
constexpr auto DefaultSpecIndex = 0;
constexpr auto DefaultFractalIndex = 0;
constexpr auto DefaultFractalNameIndex = 0;
constexpr auto ProgramName = "Mandelbulb";


int main()
{
	if (!glfwInit())
	{
		DebugPrint("Error: Glfw failed to initialize");
		return -1;
	}

#if ConstWindowSize
	GLFWwindow* mainWindow = glfwCreateWindow(screenSize.x, screenSize.y, ProgramName, nullptr, nullptr);
#else
	// glfw window creation
	glm::ivec2 screenSize = Fractal::GetMonitorSize();
	#if _DEBUG
		GLFWwindow* mainWindow = glfwCreateWindow(screenSize.x, screenSize.y, ProgramName, nullptr, nullptr);
	#else
		GLFWwindow* mainWindow = glfwCreateWindow(screenSize.x, screenSize.y, ProgramName, glfwGetPrimaryMonitor(), nullptr);
	#endif
#endif

	if (!mainWindow)
	{
		DebugPrint("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(mainWindow);

	if (glewInit() != GLEW_OK)
	{
		DebugPrint("Glew error");
		return -1;
	}

#if _DEBUG
	std::cout << glGetString(GL_VERSION) << std::endl;
#endif

#if ConstWindowSize
	Fractal* fractal = new DefaultFractal(DefaultSpecIndex, DefaultFractalIndex, DefaultFractalNameIndex, screenSize);
#else
	Fractal* fractal = new DefaultFractal(DefaultSpecIndex, DefaultFractalIndex, DefaultFractalNameIndex);
#endif

	glfwSetWindowUserPointer(mainWindow, fractal);

	// Executing the main purpose of the current run
	if (programPurpose == explore)
	{
		Fractal::RenderLoop(mainWindow, fractal);
	}
	else if (programPurpose == singleImage)
	{
		Fractal::GenerateSingleImage(mainWindow, fractal);
	}
	else
	{
		DebugPrint("I am quite unsure about my purpose in this world");
		BreakIfDebug();
	}

	// The pointer may get reassigned during runtime, with the stack allocated pointer pointing to invalid memory
	fractal = (Fractal*)glfwGetWindowUserPointer(mainWindow);
	delete fractal;

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
	return 0;
}
