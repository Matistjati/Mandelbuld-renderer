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
constexpr auto DefaultFractalNameIndex = 1;
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
		Fractal::MainLoop(mainWindow, fractal);
	}
	else if (programPurpose == singleImage)
	{
		DebugPrint("I am fully aware of my purpose, and instructions exist for fulfilling it. However, my creator deems them to be \"sloppy\" and will probably be integrated in the next version :)");
	}
	else
	{
		DebugPrint("I am quite unsure about my purpose in this world");
		BreakIfDebug();
	}


	//double dt = 0.5;
	//double pi2 = 6.28318530717958647692528676655;
	//int imageCount = int(pi2 / dt) + 1;
	//int count = 0;
	//fractal->renderShader->use();
	////fractal->zoom.value = 0.0150609445;
	////(reinterpret_cast<Fractal2D*>(fractal))->position.value.x = -0.73962032;
	////(reinterpret_cast<Fractal2D*>(fractal))->position.value.y = 0.210820600;
	//fractal->zoom.value = 1.3;
	//(reinterpret_cast<Fractal2D*>(fractal))->position.value.x = -0.23962032;
	//(reinterpret_cast<Fractal2D*>(fractal))->position.value.y = 0;
	//fractal->SetUniformLocations(fractal->renderShader);
	//fractal->SetUniforms(fractal->renderShader);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractal->explorationShader->buffers[Fractal::rectangleVertexBufferIndexName].id);
	//glBindVertexArray(fractal->explorationShader->buffers[Fractal::rectangleVertexBufferName].id);
	//std::vector<Pixel*> images = std::vector<Pixel*>(imageCount);
	//for (double time = 0; time < pi2; time+=dt, count++)
	//{
	//	fractal->time.value.SetTotalTime(time);
	//	fractal->explorationShader->SetUniform(fractal->time);



	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



	//	Pixel* data = (Pixel*)malloc(fractal->screenSize.value.x * fractal->screenSize.value.y * sizeof(Pixel));
	//	glReadPixels(0, 0, fractal->screenSize.value.x, fractal->screenSize.value.y, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//	images[count] = data;
	//	GlErrorCheck();
	//}

	//Pixel* dataOut = (Pixel*)malloc(fractal->screenSize.value.x * fractal->screenSize.value.y * sizeof(Pixel));

	//for (size_t i = 0; i < fractal->screenSize.value.x * fractal->screenSize.value.y; i++)
	//{
	//	int sumR = 0;
	//	int sumG = 0;
	//	int sumB = 0;
	//	int sumA = 0;
	//	for (size_t j = 0; j < imageCount; j++)
	//	{
	//		Pixel* cur = images[j];
	//		sumR += cur[i].r;
	//		sumG += cur[i].g;
	//		sumB += cur[i].b;
	//		sumA += cur[i].a;
	//	}
	//	dataOut[i] = Pixel(sumR / imageCount, sumG / imageCount, sumB / imageCount, sumA / imageCount);
	//}



	//Image image(fractal->screenSize.value.x, fractal->screenSize.value.y, dataOut);

	//image.FlipVertically();

	//try
	//{
	//	int count = 0;
	//	while (FileManager::FileExists("C:/users/matis/desktop/avg"+std::to_string(count)+".png"))
	//	{
	//		count++;
	//	}
	//	image.Save(std::string("C:/users/matis/desktop/avg" + std::to_string(count) + ".png").c_str());
	//}
	//catch (const std::exception& e)
	//{
	//	DebugPrint("Error saving image: " + *e.what());
	//	return -1;
	//}

	//return -1;

	// The pointer may get reassigned during runtime, with the stack allocated pointer pointing to invalid memory
	fractal = (Fractal*)glfwGetWindowUserPointer(mainWindow);
	delete fractal;

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
	return 0;
}
