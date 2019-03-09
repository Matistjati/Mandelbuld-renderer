#include <Windows.h>
#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "shader.h"

constexpr unsigned int width = 800;
constexpr unsigned int height = 600;

/*
static std::string ParseShader(const std::string& file)
{
	std::ifstream stream;

	// Set exceptions to be thrown on failure
	stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try 
	{
		stream.open(file);
	}
	catch (std::system_error& e)
	{
		std::cerr << e.code().message() << std::endl;
	}

	std::string str((std::istreambuf_iterator<char>(stream)),
		std::istreambuf_iterator<char>());

	return str;
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "failed to compile " <<
			((type == GL_VERTEX_SHADER) ? " vertex " : " fragment ") << " shader.\nError: " << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}*/

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

	Shader mainShader("resources/shaders/Vertex.shader", "resources/shaders/FragmentMandelBulb.shader");

	mainShader.use();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(VAO);

	double lastTime = glfwGetTime();

	//int vertexColorLocation = glGetUniformLocation(mainShader.id, "color");


	// render loop
	while (!glfwWindowShouldClose(window))
	{
		//float timeValue = glfwGetTime();
		//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		// this aint cheap
		glfwSetWindowTitle(window, std::to_string(1 / (glfwGetTime() - lastTime)).c_str());
		lastTime = glfwGetTime();

		// render
		glClear(GL_COLOR_BUFFER_BIT);


		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}