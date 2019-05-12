#include <Windows.h>
#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include "shader.h"



constexpr unsigned int width = 1080;
constexpr unsigned int height = 1080;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

float t = 0.1;

struct MandelInfo
{
	float power;
	float escapeRadius;
	float offsetX;
	float offsetY;
	float zoom;
	int maxIterations;
	Shader shader;
};

inline std::string GetWorkingDirectory()
{
	const int bufferSize = 256;
	char path[bufferSize];
	GetCurrentDirectory(bufferSize, path);
	return std::string(path);
	
}

int main()
{
	std::string workingDir = GetWorkingDirectory();

	workingDir = workingDir.substr(0, workingDir.find_last_of("/\\"));

	if (!SetCurrentDirectory(workingDir.c_str()))
	{
		std::cout << "Error setting working directory";
	}

	// glfw initialization
	if (!glfwInit())
	{
		return -1;
	}

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

	Shader mainShader("resources/shaders/Vertex.vs", "resources/shaders/FragmentMandel.fs");

	MandelInfo mandelInfo{ 2, 4, 0, 0, 1, 1024, mainShader };

	mandelInfo.shader.setInt("maxIterations", mandelInfo.maxIterations);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowUserPointer(window, &mandelInfo);

	mainShader.use();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(VAO);

	double lastTime = glfwGetTime();
	double startTime = lastTime;
	int tLocation = glGetUniformLocation(mainShader.id, "t");

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		//float timeValue = glfwGetTime();
		//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		glUniform1f(tLocation, glfwGetTime() - startTime);

		// this aint cheap
		glfwSetWindowTitle(window, std::to_string(1 / (glfwGetTime() - lastTime)).c_str());
		lastTime = glfwGetTime();

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

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(push)
#pragma warning(disable : 4305)

	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->offsetY += mandel->zoom * 0.1;
		mandel->shader.setFloat("offsetY", mandel->offsetY);
	}

	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->offsetY -= mandel->zoom * 0.1;
		mandel->shader.setFloat("offsetY", mandel->offsetY);
	}

	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->offsetX -= mandel->zoom * 0.1;
		mandel->shader.setFloat("offsetX", mandel->offsetX);
	}

	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->offsetX += mandel->zoom * 0.1;
		mandel->shader.setFloat("offsetX", mandel->offsetX);
	}

	if (key == GLFW_KEY_Q && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->zoom *= 0.95;
		mandel->shader.setFloat("zoom", mandel->zoom);
	}

	if (key == GLFW_KEY_E && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->zoom /= 0.95;
		mandel->shader.setFloat("zoom", mandel->zoom);
	}

	if (key == GLFW_KEY_Z && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->maxIterations++;
		mandel->shader.setInt("maxIterations", mandel->maxIterations);
		mandel->shader.setFloat("iterationLog", log(mandel->maxIterations));
	}

	if (key == GLFW_KEY_X && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->maxIterations-=1;
		mandel->shader.setInt("maxIterations", mandel->maxIterations);
		mandel->shader.setFloat("iterationLog", log(mandel->maxIterations));
	}

	if (key == GLFW_KEY_R && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->power -= 0.025;
		mandel->shader.setFloat("power", mandel->power);
		mandel->shader.setFloat("escapeRadius", pow(2, mandel->power));
	}

	if (key == GLFW_KEY_F && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->power += 0.025;
		mandel->shader.setFloat("power", mandel->power);
		mandel->shader.setFloat("escapeRadius", pow(2, mandel->power));
	}
#pragma warning(pop)
#pragma warning(pop)
}