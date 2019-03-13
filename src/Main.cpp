#include <Windows.h>
#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include "shader.h"

constexpr unsigned int width = 1080;
constexpr unsigned int height = 1080;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

struct MandelInfo
{
	float zoom;
	int maxIterations;
	float x;
	float y;
	float z;
	Shader shader;
};

int main()
{
	// glfw: initialize and configure
	if (!glfwInit())
	{
		return -1;
	}

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(width, height, "Hai", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Glew error" << std::endl;
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;


	float vertices1[30] =
	{	
		// Positions       // Colors
		 1.f,  1.f, 0.0f,  .66f, 0.0f, 0.0f,    // top right
		 1.f, -1.f, 0.0f,  0.33f, 0.33f, 0.0f,  // bottom right
		 -1.f,  1.f, 0.0f, 0.33f, 0.0f, 0.33f,  // top left,
		-1.f, -1.f, 0.0f,  0.33f, 0.33f, 0.0f   // bottom left
	};

	unsigned int indices[6] =
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
	glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), vertices1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	Shader mainShader("resources/shaders/Vertex.vs", "resources/shaders/MandelBulb.fs");

	MandelInfo mandelInfo{ 0, 0, 0.0, 0.0, 3.0, mainShader };

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowUserPointer(window, &mandelInfo);

	mainShader.use();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(VAO);

	double lastTime = glfwGetTime();


	// render loop
	while (!glfwWindowShouldClose(window))
	{
		//float timeValue = glfwGetTime();
		//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		// this aint cheap
		glfwSetWindowTitle(window, std::to_string(1 / (glfwGetTime() - lastTime)).c_str());
		lastTime = glfwGetTime();
		mainShader.setFloat("time", lastTime);

		// render
		glClear(GL_COLOR_BUFFER_BIT);


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	MandelInfo *mandel = (MandelInfo*)glfwGetWindowUserPointer(window);


	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->x += .1;
		mandel->shader.set3f("eye", mandel->x, mandel->y, mandel->z);
	}

	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->x -= .1;
		mandel->shader.set3f("eye", mandel->x, mandel->y, mandel->z);
	}

	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->y += .1;
		mandel->shader.set3f("eye", mandel->x, mandel->y, mandel->z);
	}

	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->y -= .1;
		mandel->shader.set3f("eye", mandel->x, mandel->y, mandel->z);
	}

	if (key == GLFW_KEY_Q && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->z += .5;
		mandel->shader.set3f("eye", mandel->x, mandel->y, mandel->z);
	}

	if (key == GLFW_KEY_E && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->z -= .5;
		mandel->shader.set3f("eye", mandel->x, mandel->y, mandel->z);
	}
}
