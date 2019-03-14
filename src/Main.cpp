#include <Windows.h>
#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "shader.h"
#include "Camera.h"

constexpr unsigned int width = 1080;
constexpr unsigned int height = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
bool statechanged = true;
struct MandelInfo
{
	float power;
	int maxIterations;
	float bailOut;
	float deltaTime;
	float lastX = 0;
	float lastY = 0;
	bool firstMouse = true;
	Shader shader;
	Camera camera;
	int yawLocation;
	int pitchLocation;
	int eyeLocation;
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
	Camera camera(glm::vec3(0, 0, 0));
	camera.MouseSensitivity = 0.001f;

	MandelInfo mandelInfo{ 8, 10, 1.15, 0.0, 0, 0, true, mainShader, camera};

	mandelInfo.yawLocation = glGetUniformLocation(mandelInfo.shader.id, "yaw");
	mandelInfo.pitchLocation = glGetUniformLocation(mandelInfo.shader.id, "pitch");
	mandelInfo.eyeLocation = glGetUniformLocation(mandelInfo.shader.id, "eye");

	mainShader.use();

	mandelInfo.shader.setFloat("Power", mandelInfo.power);
	mandelInfo.shader.setInt("maxIterations", mandelInfo.maxIterations);
	mandelInfo.shader.setFloat("bailout", mandelInfo.bailOut);

	glfwSetWindowUserPointer(window, &mandelInfo);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(VAO);

	double lastTime = glfwGetTime();

	int timeLocation = glGetUniformLocation(mainShader.id, "time");

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		//float timeValue = glfwGetTime();
		//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		// this aint cheap
		float deltaTime = (glfwGetTime() - lastTime);
		mandelInfo.deltaTime = deltaTime;

		glfwSetWindowTitle(window, std::to_string(1 / deltaTime).c_str());
		lastTime = glfwGetTime();
		glUniform1f(timeLocation, lastTime);

		// render
		//glClear(GL_COLOR_BUFFER_BIT);


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	MandelInfo *mandel = (MandelInfo*)glfwGetWindowUserPointer(window);
	mandel->shader.setInt("width", width);
	mandel->shader.setInt("height", height);
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	MandelInfo *mandel = (MandelInfo*)glfwGetWindowUserPointer(window);


	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::FORWARD, mandel->deltaTime);
		glUniform3f(mandel->eyeLocation, mandel->camera.Position.x, mandel->camera.Position.y, mandel->camera.Position.z);
	}

	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::BACKWARD, mandel->deltaTime);
		glUniform3f(mandel->eyeLocation, mandel->camera.Position.x, mandel->camera.Position.y, mandel->camera.Position.z);
	}

	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::LEFT, mandel->deltaTime);
		glUniform3f(mandel->eyeLocation, mandel->camera.Position.x, mandel->camera.Position.y, mandel->camera.Position.z);
	}

	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::RIGHT, mandel->deltaTime);
		glUniform3f(mandel->eyeLocation, mandel->camera.Position.x, mandel->camera.Position.y, mandel->camera.Position.z);
	}


	if (key == GLFW_KEY_Z && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->maxIterations += 1;
		mandel->shader.setInt("maxIterations", mandel->maxIterations);
	}

	if (key == GLFW_KEY_X && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->maxIterations -= 1;
		mandel->shader.setInt("maxIterations", mandel->maxIterations);
	}

	if (key == GLFW_KEY_C && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->power += 0.01;
		mandel->shader.setFloat("Power", mandel->power);
	}

	if (key == GLFW_KEY_V && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->power -= 0.01;
		mandel->shader.setFloat("Power", mandel->power);
	}

	if (key == GLFW_KEY_R && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->bailOut += 0.01;
		mandel->shader.setFloat("bailout", mandel->bailOut);
	}

	if (key == GLFW_KEY_F && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->bailOut -= 0.01;
		mandel->shader.setFloat("bailout", mandel->bailOut);
	}

	if (key == GLFW_KEY_ESCAPE && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		glfwSetWindowShouldClose(window, 1);
	}
}



void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	MandelInfo *mandel = (MandelInfo*)glfwGetWindowUserPointer(window);

	if (mandel->firstMouse)
	{
		mandel->lastX = xpos;
		mandel->lastY = ypos;
		mandel->firstMouse = false;
	}

	float xoffset = xpos - mandel->lastX;
	float yoffset = mandel->lastY - ypos; // reversed since y-coordinates go from bottom to top

	mandel->lastX = xpos;
	mandel->lastY = ypos;

	mandel->camera.ProcessMouseMovement(xoffset, yoffset);
	glUniform1f(mandel->yawLocation, mandel->camera.Yaw);
	glUniform1f(mandel->pitchLocation, mandel->camera.Pitch);
}
