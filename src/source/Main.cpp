#include <windows.h>
#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "headers/Camera.h"
#include "headers/shader.h"
#include "headers/Image.h"
#include "headers/Fractal3d.h"
#include "headers/Debug.h"


#define DefaultFractal Fractal3D

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	switch (((Fractal*)glfwGetWindowUserPointer(window))->fractalType)
	{
	default:
		DebugPrint("Case default reached in function KeyCallback");
		break;
	case FractalType::fractal2D:
		DebugPrint("Add fractal2D");
		break;
	case FractalType::fractal3D:
		((Fractal3D*)glfwGetWindowUserPointer(window))->Fractal3D::FramebufferSizeCallback(window, width, height);
		break;
	}
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	switch ((reinterpret_cast<Fractal*>(glfwGetWindowUserPointer(window)))->fractalType)
	{
	default:
		DebugPrint("Case default reached in function KeyCallback");
		break;
	case FractalType::fractal2D:
		DebugPrint("Add fractal2D");
		break;
	case FractalType::fractal3D:
		(reinterpret_cast<Fractal3D*>(glfwGetWindowUserPointer(window)))->MouseCallback(window, xpos, ypos);
		break;
	}
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (((Fractal*)glfwGetWindowUserPointer(window))->fractalType)
	{
	default:
		DebugPrint("Case default reached in function KeyCallback");
		break;
	case FractalType::fractal2D:
		DebugPrint("Add fractal2D");
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
		DebugPrint("Case default reached in function KeyCallback");
		break;
	case FractalType::fractal2D:
		DebugPrint("Add fractal2D");
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

	// glfw window creation
	glm::ivec2 screenSize = Fractal::GetMonitorSize();
#if _DEBUG
	GLFWwindow* mainWindow = glfwCreateWindow(screenSize.x, screenSize.y, "Mandelbulb", NULL, NULL);
#else
	GLFWwindow* mainWindow = glfwCreateWindow(screenSize.x, screenSize.y, "Mandelbulb", glfwGetPrimaryMonitor(), NULL);
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

	std::cout << glGetString(GL_VERSION) << std::endl;


	const float vertices[12] =
	{
		// Positions
		1.f,  1.f, 0.0f,  // top right
		1.f, -1.f, 0.0f,  // bottom right
		-1.f,  1.f, 0.0f, // top left,
		-1.f, -1.f, 0.0f,  // bottom left
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

	Fractal* fractal = new DefaultFractal(0, "shaders/3D/Fractals/MandelboxSpecs.fs", "shaders/3D/Fractals/Mandelbox.fs");
	fractal->fractalType = FractalType::fractal3D;

	glfwSetWindowUserPointer(mainWindow, fractal);


	glfwSetFramebufferSizeCallback(mainWindow, FrameBufferSizeCallback);
	glfwSetCursorPosCallback(mainWindow, MouseCallback);
	glfwSetKeyCallback(mainWindow, KeyCallback);
	glfwSetScrollCallback(mainWindow, ScrollCallBack);

	glViewport(0, 0, fractal->screenSize.value.x, fractal->screenSize.value.y);

	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	fractal->explorationShader.use();

	GlErrorCheck();

	// render loop
	while (!glfwWindowShouldClose(mainWindow))
	{
		fractal->Update();

#if _DEBUG
		// Set the window title to our fps
		glfwSetWindowTitle(mainWindow, std::to_string(1 / fractal->time.deltaTime).c_str());
#endif

		// render, we use ray marching inside the fragment shader
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
		fractal->HandleKeyInput();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}
