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

#define FractalType Fractal3D


void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	((FractalType*)glfwGetWindowUserPointer(window))->FramebufferSizeCallback(window, width, height);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	((FractalType*)glfwGetWindowUserPointer(window))->MouseCallback(window, xpos, ypos);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	((FractalType*)glfwGetWindowUserPointer(window))->KeyCallback(window, key, scancode, action, mods);
}

int main()
{
	if (!glfwInit())
	{
		DebugPrint("Error: Glfw failed to initialize");
		return -1;
	}

	// glfw window creation
	GLFWwindow* mainWindow = glfwCreateWindow(Fractal::DefaultWidth, Fractal::DefaultHeight, "Mandelbulb", NULL, NULL);
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

	FractalType fractal = FractalType(1, "resources/shaders/3D/Fractals/MandelboxSpecs.fs", "resources/shaders/3D/Fractals/Mandelbox.fs");


	glfwSetWindowUserPointer(mainWindow, &fractal);


	glfwSetFramebufferSizeCallback(mainWindow, FrameBufferSizeCallback);
	glfwSetCursorPosCallback(mainWindow, MouseCallback);
	glfwSetKeyCallback(mainWindow, KeyCallback);

	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	fractal.explorationShader.use();

	GlErrorCheck();

	// render loop
	while (!glfwWindowShouldClose(mainWindow))
	{
		fractal.Update();

#if _DEBUG
		// Set the window title to our fps
		glfwSetWindowTitle(mainWindow, std::to_string(1 / fractal.time.deltaTime).c_str());
#endif

		// render, we use ray marching inside the fragment shader
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
		fractal.HandleKeyInput();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}
