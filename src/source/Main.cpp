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

#define ConstWindowSize 0
#if ConstWindowSize
	#define ScreenSize 0

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

#define DefaultFractal Fractal2D
constexpr auto DefaultSpecIndex = 0;
constexpr auto DefaultFractalIndex = 0;
constexpr auto DefaultFractalNameIndex = 0;
constexpr auto ProgramName = "Mandelbulb";

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	switch ((reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)))->fractalType)
	{
	default:
		DebugPrint("Case default reached in function FrameBufferSizeCallback");
		break;
	case FractalType::fractal2D:
		(reinterpret_cast<Fractal2D*>(glfwGetWindowUserPointer(window)))->FramebufferSizeCallback(window, width, height);
		break;
	case FractalType::fractal3D:
		(reinterpret_cast<Fractal3D*>(glfwGetWindowUserPointer(window)))->FramebufferSizeCallback(window, width, height);
		break;
	}
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	switch ((reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)))->fractalType)
	{
	default:
		DebugPrint("Case default reached in function MouseCallback");
		break;
	case FractalType::fractal2D:
		(reinterpret_cast<Fractal2D*>(glfwGetWindowUserPointer(window)))->MouseCallback(window, xpos, ypos);
		break;
	case FractalType::fractal3D:
		(reinterpret_cast<Fractal3D*>(glfwGetWindowUserPointer(window)))->MouseCallback(window, xpos, ypos);
		break;
	}
}


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS && (mods & GLFW_MOD_ALT) == GLFW_MOD_ALT)
	{
		Fractal* fractal = reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window));
		bool update = true;
		switch (key)
		{
		default:
			update = false;
			break;
		case GLFW_KEY_Q:
			fractal->specIndex++;
			fractal->fractalIndex = 0;
			break;
		case GLFW_KEY_A:
			fractal->specIndex--;
			fractal->specIndex = std::max(fractal->specIndex, 0);
			fractal->fractalIndex = 0;
			break;

		case GLFW_KEY_W:
			fractal->fractalIndex++;
			break;
		case GLFW_KEY_S:
			fractal->fractalIndex--;
			fractal->fractalIndex = std::max(fractal->fractalIndex, 0);
			break;

		case GLFW_KEY_E:
			fractal->fractalNameIndex++;
			fractal->fractalIndex = 0;
			fractal->specIndex = 0;
			fractal->SetFractalNameFromIndex(&fractal->fractalNameIndex, fractal->GetFractalFolderPath());
			break;
		case GLFW_KEY_D:
			fractal->fractalNameIndex--;
			fractal->fractalIndex = 0;
			fractal->specIndex = 0;
			fractal->SetFractalNameFromIndex(&fractal->fractalNameIndex, fractal->GetFractalFolderPath());
			break;

		case GLFW_KEY_R:
			fractal->fractalNameIndex = 0;
			fractal->fractalIndex = 0;
			fractal->specIndex = 0;
			if (fractal->fractalType == FractalType::fractal2D)
			{
				glfwSetWindowUserPointer(window, new Fractal3D(fractal->specIndex, fractal->fractalIndex, fractal->fractalNameIndex, fractal->screenSize.value));
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else
			{
				glfwSetWindowUserPointer(window, new Fractal2D(fractal->specIndex, fractal->fractalIndex, fractal->fractalNameIndex, fractal->screenSize.value));
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			delete fractal;
			fractal = (Fractal*)glfwGetWindowUserPointer(window);
			update = true;
			break;
		}
		if (update)
		{
			fractal->UpdateFractalShader();
			return;
		}
	}
	switch (((Fractal*)glfwGetWindowUserPointer(window))->fractalType)
	{
	default:
		DebugPrint("Case default reached in function KeyCallback");
		break;
	case FractalType::fractal2D:
		(reinterpret_cast<Fractal2D*>(glfwGetWindowUserPointer(window)))->KeyCallback(window, key, scancode, action, mods);
		break;
	case FractalType::fractal3D:
		(reinterpret_cast<Fractal3D*>(glfwGetWindowUserPointer(window)))->KeyCallback(window, key, scancode, action, mods);
		break;
	}
}

void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	switch (((Fractal*)glfwGetWindowUserPointer(window))->fractalType)
	{
	default:
		DebugPrint("Case default reached in function ScrollCallBack");
		break;
	case FractalType::fractal2D:
		(reinterpret_cast<Fractal2D*>(glfwGetWindowUserPointer(window)))->ScrollCallback(window, xoffset, yoffset);
		break;
	case FractalType::fractal3D:
		(reinterpret_cast<Fractal3D*>(glfwGetWindowUserPointer(window)))->ScrollCallback(window, xoffset, yoffset);
		break;
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


	glfwSetFramebufferSizeCallback(mainWindow, FrameBufferSizeCallback);
	glfwSetCursorPosCallback(mainWindow, MouseCallback);
	glfwSetKeyCallback(mainWindow, KeyCallback);
	glfwSetScrollCallback(mainWindow, ScrollCallBack);

	glViewport(0, 0, fractal->screenSize.value.x, fractal->screenSize.value.y);

	if (fractal->fractalType == fractal3D)
	{
		glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	
	fractal->explorationShader->use(); 
	GlErrorCheck();
	// render loop
	while (!glfwWindowShouldClose(mainWindow))
	{
		fractal->time.value.PollTime();
		fractal->explorationShader->SetUniform(fractal->time);

		fractal->Update();

#if _DEBUG
		// Set the window title to our fps
		std::string title = std::to_string(1 / fractal->time.value.GetDeltaTime());
		glfwSetWindowTitle(mainWindow, title.c_str());
#endif

		fractal->explorationShader->use();
		if (fractal->explorationShader->type == fragment)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractal->explorationShader->buffers[Fractal::rectangleVertexBufferIndexName].id);
			glBindVertexArray(fractal->explorationShader->buffers[Fractal::rectangleVertexBufferName].id);
			// rendering, we use ray marching inside the fragment shader
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		else if (fractal->explorationShader->type == compute)
		{
			reinterpret_cast<ComputeShader*>(fractal->explorationShader)->Invoke(fractal->screenSize.value);
		}


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
		fractal = (Fractal*)glfwGetWindowUserPointer(mainWindow);
		fractal->HandleKeyInput();
	}

	delete fractal;

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
	return 0;
}
