// Don't let windows.h define macros which will mess up some compilation later
#define NOMINMAX
#include <windows.h>

#define NANOGUI_GLAD
#if defined(NANOGUI_GLAD)
	#if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
		#define GLAD_GLAPI_EXPORT
	#endif

	#include <glad/glad.h>
	#else
		#if defined(__APPLE__)
			#define GLFW_INCLUDE_GLCOREARB
		#else
			#define GL_GLEXT_PROTOTYPES
	#endif
#endif

#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>

#include <iostream>
#include <string>
#include <algorithm>

#include "headers/Camera.h"
#include "headers/shader.h"
#include "headers/Image.h"
#include "headers/FileManager.h"
#include "headers/Fractal.h"
#include "headers/Debug.h"
#include "headers/GUI.h"
#include "nanogui/layout.h"

enum class Purpose
{
	explore,
	singleImage,
	imageSequence
};

constexpr Purpose programPurpose = Purpose::explore;

// Screensize
#define ConstWindowSize 0
#if ConstWindowSize
	#define ScreenSize 2

	// Common screen resolutions
	#if ScreenSize == 0
		const glm::vec2 screenSize = { 800, 450 };
	#elif ScreenSize == 1
		const glm::vec2 screenSize = { 1280, 720 };
	#elif ScreenSize == 2
		const glm::vec2 screenSize = { 1920, 1080 };
	#elif ScreenSize == 3
		const glm::vec2 screenSize = { 2560, 1440 };
	#elif ScreenSize == 4
		const glm::vec2 screenSize = { 4096, 2160 };
	#elif ScreenSize == 5
		const glm::vec2 screenSize = { 6400, 4096 };
	#elif ScreenSize == 6
		const glm::vec2 screenSize = { 7680, 4320 };
	#endif
#endif

// Starting fractal
constexpr auto DefaultFractalIndex = 0;
constexpr auto DefaultSpecIndex = 0;
constexpr auto DefaultFractalNameIndex = 0;
#define DefaultFractalType FractalType::fractal2D
constexpr auto ProgramName = "Mandelbulb";

void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam)
{
	if (type == GL_DEBUG_TYPE_OTHER)
	{
		return;
	}

	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);

	if (type == GL_DEBUG_TYPE_ERROR)
	{
		BreakIfDebug();
	}
}

int main()
{
	if (!glfwInit())
	{
		DebugPrint("Error: Glfw failed to initialize");
		return -1;
	}

#if ConstWindowSize
	GLFWwindow* mainWindow = glfwCreateWindow(int(screenSize.x), int(screenSize.y), ProgramName, nullptr, nullptr);
	glfwSetWindowSizeLimits(mainWindow, 0, 0, int(screenSize.x), int(screenSize.y));
	glfwSetWindowSize(mainWindow, int(screenSize.x), int(screenSize.y));
	Fractal::screenSize = Uniform<glm::vec2>(screenSize, "screenSize", -1);
#else
	// glfw window creation
	glm::vec2 screenSize = Fractal::GetMonitorSize();
	Fractal::screenSize = Uniform<glm::vec2>(screenSize, "screenSize", -1);
	#if _DEBUG
		GLFWwindow* mainWindow = glfwCreateWindow((int)screenSize.x, (int)screenSize.y, ProgramName, nullptr, nullptr);
		glfwSetWindowSizeLimits(mainWindow, 0, 0, int(screenSize.x), int(screenSize.y));
		glfwSetWindowSize(mainWindow, int(screenSize.x), int(screenSize.y));
	#else
		GLFWwindow* mainWindow = glfwCreateWindow(screenSize.x, screenSize.y, ProgramName, nullptr, nullptr);
		//GLFWwindow* mainWindow = glfwCreateWindow(screenSize.x, screenSize.y, ProgramName, glfwGetPrimaryMonitor(), nullptr);
	#endif
#endif

	if (!mainWindow)
	{
		DebugPrint("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(mainWindow);

	Fractal::window = mainWindow;


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

#if _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	std::cout << glGetString(GL_VERSION) << std::endl;
#endif

	// Create a nanogui screen and pass the glfw pointer to initialize
	Fractal* fractal = new Fractal(DefaultFractalType, DefaultSpecIndex, DefaultFractalIndex, DefaultFractalNameIndex, screenSize);
	fractal->Init();

	glfwSetWindowUserPointer(mainWindow, fractal);

	// Executing the main purpose of the current run
	if (programPurpose == Purpose::explore)
	{
		Fractal::RenderLoop(mainWindow, fractal);
	}
	else if (programPurpose == Purpose::singleImage)
	{
		Fractal::GenerateSingleImage(mainWindow, fractal);
	}
	else if (programPurpose == Purpose::imageSequence)
	{
		Fractal::ImageSequence(mainWindow, fractal);
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
