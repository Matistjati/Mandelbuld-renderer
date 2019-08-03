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
#include "headers/Fractal3d.h"
#include "headers/Fractal2D.h"
#include "headers/Debug.h"

enum Purpose
{
	singleImage,
	explore,
};

constexpr Purpose programPurpose = explore;

// Screensize
#define ConstWindowSize 1
#if ConstWindowSize
	#define ScreenSize 2

	// Common screen resolutions
	#if ScreenSize == 0
		constexpr auto ConstScreenSizeX = 500;
		constexpr auto ConstScreenSizeY = 500;
	#elif ScreenSize == 1
		constexpr auto ConstScreenSizeX = 1280;
		constexpr auto ConstScreenSizeY = 720;
	#elif ScreenSize == 2
		constexpr auto ConstScreenSizeX = 1920;
		constexpr auto ConstScreenSizeY = 1080;
	#elif ScreenSize == 3
		constexpr auto ConstScreenSizeX = 2560;
		constexpr auto ConstScreenSizeY = 1440;
	#elif ScreenSize == 4
		constexpr auto ConstScreenSizeX = 3840;
		constexpr auto ConstScreenSizeY = 2160;
	#elif ScreenSize == 5
		constexpr auto ConstScreenSizeX = 7680;
		constexpr auto ConstScreenSizeY = 4320;
	#endif
#endif

// Starting fractal
#define DefaultFractal Fractal2D
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
	GLFWwindow* mainWindow = glfwCreateWindow(ConstScreenSizeX, ConstScreenSizeY, ProgramName, NULL, NULL);
#else
	// glfw window creation
	glm::ivec2 screenSize = Fractal::GetMonitorSize();
#if _DEBUG
	GLFWwindow* mainWindow = glfwCreateWindow(screenSize.x, screenSize.y, ProgramName, NULL, NULL);
#else
	GLFWwindow* mainWindow = glfwCreateWindow(screenSize.x, screenSize.y, ProgramName, glfwGetPrimaryMonitor(), NULL);
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
	Fractal* fractal = new DefaultFractal(DefaultSpecIndex, DefaultFractalIndex, DefaultFractalNameIndex, glm::ivec2(ConstScreenSizeX, ConstScreenSizeY));
#else
	Fractal* fractal = new DefaultFractal(DefaultSpecIndex, DefaultFractalIndex, DefaultFractalNameIndex);
#endif

	glfwSetWindowUserPointer(mainWindow, fractal);

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
