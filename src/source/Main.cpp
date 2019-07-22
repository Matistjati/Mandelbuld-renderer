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

constexpr auto ConstScreenSizeX = 500;
constexpr auto ConstScreenSizeY = 500;
#define ConstWindowSize 0

#define DefaultFractal Fractal3D
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


	const float vertices[12] =
	{
		// Positions
		 1.f,  1.f, 0.0f, // top right
		 1.f, -1.f, 0.0f, // bottom right
		-1.f,  1.f, 0.0f, // top left
		-1.f, -1.f, 0.0f, // bottom left
	};

	const unsigned int indices[6] =
	{
		0, 1, 2,
		1, 2, 3
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(VAO);

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

	GlErrorCheck();


	Shader c("C:/Users/Matis/source/repos/Mandelbulb/shaders/mandelbrotcompute.fs", true);

	glUseProgram(c.id);

	GLuint buffHandle;
	glGenBuffers(1, &buffHandle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffHandle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1920 * 1080 * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);


	GlErrorCheck();
	glUseProgram(c.id);
	glDispatchCompute(1920 / 8, 1080 / 8, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffHandle);
	GlErrorCheck();

	GlErrorCheck();

	glm::vec4* ptr;
	ptr = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	GlErrorCheck();

	Pixel* rgb = (Pixel*)malloc(1920 * 1080 * sizeof(Pixel));
	if (rgb == nullptr)
	{
		return -1;
	}

	for (size_t i = 0; i < 1920 * (1080); i++)
	{
		Pixel* image = &rgb[i];
		glm::vec4* buffer = &ptr[i];

		image->red = (int)(buffer->x * 255);
		image->green = (int)(buffer->y * 255);
		image->blue = (int)(buffer->z * 255);
		image->alpha = (int)(buffer->a * 255);
	}

	Image image(1920, 1080, rgb);

	try
	{
		image.Save("C:/Users/Matis/source/repos/Mandelbulb/TestImage/test.png");
		DebugPrint("Successfully saved image \"test.png\"");
	}
	catch (const std::exception& e)
	{
		DebugPrint("Error saving image: " + *e.what());
		return -1;
	}






	// render loop
	while (!glfwWindowShouldClose(mainWindow))
	{
		fractal->time.value.PollTime();
		fractal->explorationShader->SetUniform(fractal->time);

		fractal->Update();

#if _DEBUG
		// Set the window title to our fps
		glfwSetWindowTitle(mainWindow, std::to_string(1 / fractal->time.value.GetDeltaTime()).c_str());
#endif

		// render, we use ray marching inside the fragment shader
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
		fractal = (Fractal*)glfwGetWindowUserPointer(mainWindow);
		fractal->HandleKeyInput();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	delete fractal;

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}
