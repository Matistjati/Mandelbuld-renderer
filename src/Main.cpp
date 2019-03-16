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

struct MandelInfo
{
	float power;
	int maxIterations;
	float deltaTime;
	float lastX = 0;
	float lastY = 0;
	bool firstMouse = true;
	Shader shader;
	Camera camera;
	int yawMatrixLocation;
	int pitchMatrixLocation;
	int eyeLocation;
	int powerLocation;
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


	float vertices1[12] =
	{	
		// Positions
		 1.f,  1.f, 0.0f,  // top right
		 1.f, -1.f, 0.0f,  // bottom right
		 -1.f,  1.f, 0.0f, // top left,
		-1.f, -1.f, 0.0f,  // bottom left
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
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertices1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(VAO);

	Shader mainShader("resources/shaders/Vertex.vs", "resources/shaders/MandelBulb.fs");
	Camera camera(glm::vec3(5, 5, 5));
	camera.MouseSensitivity = 0.1f;
	camera.MovementSpeed = 1;

	MandelInfo mandelInfo{ 8, 10, 0.0, 0, 0, true, mainShader, camera};

	mandelInfo.pitchMatrixLocation = glGetUniformLocation(mandelInfo.shader.id, "pitchMatrix");
	mandelInfo.yawMatrixLocation = glGetUniformLocation(mandelInfo.shader.id, "yawMatrix");
	mandelInfo.eyeLocation = glGetUniformLocation(mandelInfo.shader.id, "eye");
	mandelInfo.powerLocation = glGetUniformLocation(mandelInfo.shader.id, "power");

	mainShader.use();

	glfwSetWindowUserPointer(window, &mandelInfo);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	float lastTime = static_cast<float>(glfwGetTime());

	int timeLocation = glGetUniformLocation(mainShader.id, "time");

	mandelInfo.shader.setFloat("Power", mandelInfo.power);
	mandelInfo.shader.setInt("maxIterations", mandelInfo.maxIterations);
	mandelInfo.shader.set3f(mandelInfo.eyeLocation, mandelInfo.camera.position);
	mandelInfo.shader.setMat2(mandelInfo.yawMatrixLocation, mandelInfo.camera.GetYawMatrix2());
	mandelInfo.shader.setMat2(mandelInfo.pitchMatrixLocation, mandelInfo.camera.GetPitchMatrix2());

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// this aint cheap
		float deltaTime = static_cast<float>(glfwGetTime() - lastTime);
		mandelInfo.deltaTime = deltaTime;

		glfwSetWindowTitle(window, std::to_string(1 / deltaTime).c_str());
		lastTime = static_cast<float>(glfwGetTime());
		glUniform1f(timeLocation, lastTime);

		// render
		//glClear(GL_COLOR_BUFFER_BIT);


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);

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
		mandel->shader.set3f(mandel->eyeLocation, mandel->camera.position);
	}

	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::BACKWARD, mandel->deltaTime);
		mandel->shader.set3f(mandel->eyeLocation, mandel->camera.position);
	}

	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::LEFT, mandel->deltaTime);
		mandel->shader.set3f(mandel->eyeLocation, mandel->camera.position);
	}

	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::RIGHT, mandel->deltaTime);
		mandel->shader.set3f(mandel->eyeLocation, mandel->camera.position);
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
		mandel->power += 0.01f;
		mandel->shader.setFloat(mandel->powerLocation, mandel->power);
	}

	if (key == GLFW_KEY_V && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->power -= 0.01f;
		mandel->shader.setFloat(mandel->powerLocation, mandel->power);
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		mandel->camera.movementReverse *= -1;
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
		mandel->lastX = static_cast<float>(xpos);
		mandel->lastY = static_cast<float>(ypos);
		mandel->firstMouse = false;
	}

	float xoffset = static_cast<float>(xpos - mandel->lastX);
	float yoffset = static_cast<float>(mandel->lastY - ypos); // reversed since y-coordinates go from bottom to top

	mandel->lastX = static_cast<float>(xpos);
	mandel->lastY = static_cast<float>(ypos);

	mandel->camera.ProcessMouseMovement(xoffset, yoffset);
	mandel->shader.setMat2(mandel->pitchMatrixLocation, mandel->camera.GetPitchMatrix2());
	mandel->shader.setMat2(mandel->yawMatrixLocation, mandel->camera.GetYawMatrix2());
}
