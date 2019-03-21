#include <Windows.h>
#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "headers/Camera.h"
#include "headers/shader.h"

constexpr unsigned int width = 1920;
constexpr unsigned int height = 1080;
constexpr int startPower = 8;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

struct Locations
{
	int yawMatrix;
	int pitchMatrix;
	int eye;
	int power;
	int sun;
};

struct MandelInfo
{
	float power;
	float deltaTime;
	int worldFlip;
	float lastX = 0;
	float lastY = 0;
	bool firstMouse = true;
	Shader shader;
	Camera camera;
	Locations location;
	float genericParameter;
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
	Camera camera(glm::vec3(1.8, 0.8, -0.6), 169, -14);
	camera.MouseSensitivity = 0.1f;
	camera.MovementSpeed = 3;

	MandelInfo mandelInfo{ startPower, 0, -1, 0, 0, true, mainShader, camera, Locations{}, 1. };

	mandelInfo.location.pitchMatrix = glGetUniformLocation(mandelInfo.shader.id, "pitchMatrix");
	mandelInfo.location.yawMatrix = glGetUniformLocation(mandelInfo.shader.id, "yawMatrix");
	mandelInfo.location.eye = glGetUniformLocation(mandelInfo.shader.id, "eye");
	mandelInfo.location.power = glGetUniformLocation(mandelInfo.shader.id, "power");
	mandelInfo.location.sun = glGetUniformLocation(mandelInfo.shader.id, "sun");

	mainShader.use();

	mandelInfo.shader.setFloat("power", mandelInfo.power);
	mandelInfo.shader.set3f(mandelInfo.location.eye, mandelInfo.camera.position);
	mandelInfo.shader.setMat2(mandelInfo.location.yawMatrix, mandelInfo.camera.GetYawMatrix2());
	mandelInfo.shader.setMat2(mandelInfo.location.pitchMatrix, mandelInfo.camera.GetPitchMatrix2());
	mandelInfo.shader.setInt("width", width);
	mandelInfo.shader.setInt("height", height);

	glfwSetWindowUserPointer(window, &mandelInfo);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	float lastTime = static_cast<float>(glfwGetTime());

	int timeLocation = glGetUniformLocation(mainShader.id, "time");

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// Deltatime
		float time = static_cast<float>(glfwGetTime());
		float deltaTime = (time -lastTime);
		lastTime = time;
		mandelInfo.deltaTime = deltaTime;

		// Set the window title to our fps
		glfwSetWindowTitle(window, std::to_string(1 / deltaTime).c_str());
		// Set time in shader
		glUniform1f(timeLocation, lastTime);

		
		// Move sun in shader
		mandelInfo.shader.set3f(mandelInfo.location.sun, glm::normalize(
			glm::vec3(sin(time * 0.25),
				std::abs(sin(time * 0.1)) * -1,
				cos(time * 0.25))));


		// render, we use ray marching inside the fragment shader
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
		mandel->camera.ProcessKeyboard(Camera_Movement::forward, mandel->deltaTime);
		mandel->shader.set3f(mandel->location.eye, mandel->camera.position);
	}
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::back, mandel->deltaTime);
		mandel->shader.set3f(mandel->location.eye, mandel->camera.position);
	}
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::left, mandel->deltaTime);
		mandel->shader.set3f(mandel->location.eye, mandel->camera.position);
	}
	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::right, mandel->deltaTime);
		mandel->shader.set3f(mandel->location.eye, mandel->camera.position);
	}
	if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::up, mandel->deltaTime);
		mandel->shader.set3f(mandel->location.eye, mandel->camera.position);
	}
	if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.ProcessKeyboard(Camera_Movement::down, mandel->deltaTime);
		mandel->shader.set3f(mandel->location.eye, mandel->camera.position);
	}


	if (key == GLFW_KEY_Z && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->worldFlip *= -1;
		mandel->shader.setInt("worldFlip", mandel->worldFlip);
	}
	if (key == GLFW_KEY_X && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->camera.movementReverse *= -1;
	}


	if (key == GLFW_KEY_C && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->power += 0.03f;
		mandel->shader.setFloat(mandel->location.power, mandel->power);
	}
	if (key == GLFW_KEY_V && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->power -= 0.03f;
		mandel->shader.setFloat(mandel->location.power, mandel->power);
	}


	if (key == GLFW_KEY_Q && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->genericParameter += 0.1f;
		mandel->shader.setFloat("genericParameter", mandel->genericParameter);
	}
	if (key == GLFW_KEY_E && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		mandel->genericParameter -= 0.1f;
		mandel->shader.setFloat("genericParameter", mandel->genericParameter);
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
	mandel->shader.setMat2(mandel->location.pitchMatrix, mandel->camera.GetPitchMatrix2());
	mandel->shader.setMat2(mandel->location.yawMatrix, mandel->camera.GetYawMatrix2());
}
