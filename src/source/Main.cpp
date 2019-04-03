#include <windows.h>
#include <Shlwapi.h>
#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "headers/Camera.h"
#include "headers/shader.h"
#include "headers/Mandelbulb.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

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
#if _DEBUG
	std::cout << "Initial working directory: " << workingDir << std::endl;
#endif

	workingDir = workingDir.substr(0, workingDir.find_last_of("/\\"));
#if _DEBUG
	std::cout << "New working directory: " << workingDir << std::endl;
#endif

	if (!SetCurrentDirectory(workingDir.c_str()))
	{
		std::cout << "Error setting working directory";
	}

	// glfw initialization
	if (!glfwInit())
	{
		return -1;
	}

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(Mandelbulb::DefaultWidth, Mandelbulb::DefaultHeight, "Mandelbulb", NULL, NULL);
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

	Shader explorationShader("resources/shaders/Vertex.vs", "resources/shaders/MandelBulb.fs");

	Shader renderShader("resources/shaders/Vertex.vs", "resources/shaders/MandelBulbRender.fs");
	//renderShader.uniforms = new unsigned int[Locations::count];

	Camera camera = (Camera(glm::vec3(1.8f, 0.8f, -0.6f), // Position
		169, -14, 0.001f, // Yaw, pitch, roll
		0.1f, 3, 200) // mouseSensitivity, movementSpeed, rollSpeed
		);

	Mandelbulb mandel = Mandelbulb(Mandelbulb::defaultPower, explorationShader, renderShader, camera);


	glfwSetWindowUserPointer(window, &mandel);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	explorationShader.use();

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		mandel.time.PollTime();

#if _DEBUG
		// Set the window title to our fps
		glfwSetWindowTitle(window, std::to_string(1 / mandel.time.deltaTime).c_str());
#endif
		
		// Move sun in shader
		double time = glfwGetTime();
		mandel.explorationShader.set3f(explorationShader.uniforms[Locations::sun], glm::normalize(
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
	Mandelbulb *mandel = (Mandelbulb*)glfwGetWindowUserPointer(window);
	mandel->explorationShader.setInt("width", width);
	mandel->explorationShader.setInt("height", height);

	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_REPEAT && action != GLFW_PRESS)
	{
		return;
	}

	Mandelbulb *mandel = (Mandelbulb*)glfwGetWindowUserPointer(window);

	switch (key)
	{
		// Exit
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, 1);
		break;

		// WASD movement
	case GLFW_KEY_W:
		mandel->camera.ProcessMovement(Camera_Movement::forward, static_cast<float>(mandel->time.deltaTime));
		mandel->explorationShader.set3f(mandel->explorationShader.uniforms[Locations::eye], mandel->camera.position);
		break;
	case GLFW_KEY_S:
		mandel->camera.ProcessMovement(Camera_Movement::back, static_cast<float>(mandel->time.deltaTime));
		mandel->explorationShader.set3f(mandel->explorationShader.uniforms[Locations::eye], mandel->camera.position);
		break;
	case GLFW_KEY_A:
		mandel->camera.ProcessMovement(Camera_Movement::left, static_cast<float>(mandel->time.deltaTime));
		mandel->explorationShader.set3f(mandel->explorationShader.uniforms[Locations::eye], mandel->camera.position);
		break;
	case GLFW_KEY_D:
		mandel->camera.ProcessMovement(Camera_Movement::right, static_cast<float>(mandel->time.deltaTime));
		mandel->explorationShader.set3f(mandel->explorationShader.uniforms[Locations::eye], mandel->camera.position);
		break;

		// Up and down
	case GLFW_KEY_SPACE:
		mandel->camera.ProcessMovement(Camera_Movement::up, static_cast<float>(mandel->time.deltaTime));
		mandel->explorationShader.set3f(mandel->explorationShader.uniforms[Locations::eye], mandel->camera.position);
		break;
	case GLFW_KEY_LEFT_SHIFT:
		mandel->camera.ProcessMovement(Camera_Movement::down, static_cast<float>(mandel->time.deltaTime));
		mandel->explorationShader.set3f(mandel->explorationShader.uniforms[Locations::eye], mandel->camera.position);
		break;

		// World stuff
	case GLFW_KEY_Z:
		mandel->camera.worldFlip *= -1;
		mandel->explorationShader.setInt("worldFlip", mandel->camera.worldFlip);
		break;
	case GLFW_KEY_X:
		mandel->camera.movementReverse *= -1;
		break;


		// Changing the power of the fractal
	case GLFW_KEY_C:
		mandel->power += 0.03f;
		mandel->explorationShader.setFloat(mandel->explorationShader.uniforms[Locations::power], mandel->power);
		break;
	case GLFW_KEY_V:
		mandel->power -= 0.03f;
		mandel->explorationShader.setFloat(mandel->explorationShader.uniforms[Locations::power], mandel->power);
		break;
	
	
		// Camera roll
	case GLFW_KEY_Q:
		mandel->camera.ProcessRoll(static_cast<float>(mandel->camera.rollSpeed * mandel->time.deltaTime));
		mandel->explorationShader.setMat2(mandel->explorationShader.uniforms[Locations::rollMatrix], mandel->camera.GetRollMatrix2());
		break;
	case GLFW_KEY_E:
		mandel->camera.ProcessRoll(-static_cast<float>(mandel->camera.rollSpeed * mandel->time.deltaTime));
		mandel->explorationShader.setMat2(mandel->explorationShader.uniforms[Locations::rollMatrix], mandel->camera.GetRollMatrix2());
		break;


		// Debugging
		/*
	case GLFW_KEY_R:
		mandel->genericParameter += 0.1f;
		mandel->shader.setFloat("genericParameter", mandel->genericParameter);
		break;
	case GLFW_KEY_F:
		mandel->genericParameter -= 0.1f;
		mandel->shader.setFloat("genericParameter", mandel->genericParameter);
		break;
		*/

	default:
		break;
	}

	// Ctrl s
	if (key == GLFW_KEY_S && (mods && GLFW_MOD_CONTROL) == 1)
	{
		mandel->renderShader.use();
		mandel->SetUniforms(mandel->renderShader);
		mandel->SetUniforms(mandel->renderShader);
	}
}



void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	Mandelbulb *mandel = (Mandelbulb*)glfwGetWindowUserPointer(window);
	mandel->ProcessMouse(glm::vec2(xpos, ypos));
}
