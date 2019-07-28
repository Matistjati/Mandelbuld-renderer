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

#define ConstWindowSize 1
#if ConstWindowSize
	#define ScreenSize 1

	// Common screen resolutions
	#if ScreenSize == 0
		constexpr auto ConstScreenSizeX = 500;
		constexpr auto ConstScreenSizeY = 500;
	#elif ScreenSize == 1
		constexpr auto ConstScreenSizeX = 1280;
		constexpr auto ConstScreenSizeY = 720;
	#elif ScreenSize == 2
		constexpr auto ConstScreenSizeX = 1920;
		constexpr auto ConstScreenSizeY = 1080;
	#elif ScreenSize == 3
		constexpr auto ConstScreenSizeX = 2560;
		constexpr auto ConstScreenSizeY = 1440;
	#elif ScreenSize == 4
		constexpr auto ConstScreenSizeX = 3840;
		constexpr auto ConstScreenSizeY = 2160;
	#elif ScreenSize == 5
		constexpr auto ConstScreenSizeX = 7680;
		constexpr auto ConstScreenSizeY = 4320;
	#endif
#endif

#define DefaultFractal Fractal2D
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


	//Shader c("shaders/mandelbrotcompute.fs", true);
	//Shader s("shaders/Rectangle.glsl", "shaders/buddhaDisplay.fs", true);

	//glUseProgram(c.id);
	//const int x = fractal->screenSize.value.x;
	//const int y = fractal->screenSize.value.y;
	//GLuint buffHandle;
	//glGenBuffers(1, &buffHandle);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffHandle);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, x * y * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);

	//int computeBufferIndex = glGetProgramResourceIndex(c.id, GL_SHADER_STORAGE_BLOCK, "densityMap");
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, computeBufferIndex, buffHandle);


	///*GLuint importance;
	//glGenBuffers(1, &importance);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, importance);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, importance);
	//std::vector<glm::vec2> pos(x* y);
	//for (size_t i = 0; i < x; i++)
	//{
	//	for (size_t j = 0; j < y; j++)
	//	{
	//		pos[i * y + j] = glm::normalize(glm::vec2((float(i)/float(x))*2-1, (float(j) / float(y)) * 2 - 1))*2.f;
	//	}
	//}
	//glBufferData(GL_SHADER_STORAGE_BUFFER, x * y * sizeof(glm::vec2), &pos[0], GL_DYNAMIC_DRAW);*/

	//glUseProgram(s.id);
	//int sizeLocationFragment = glGetUniformLocation(c.id, "screenSize");
	//glUniform2f(sizeLocationFragment, (float)x, (float)y);
	//GlErrorCheck();

	//

	//glUseProgram(c.id);
	//int timeLocation = glGetUniformLocation(c.id, "time");
	//int sizeLocation = glGetUniformLocation(c.id, "screenSize");

	//glUniform2f(sizeLocation, (float)x, (float)y);

	//GlErrorCheck();
	//GlErrorCheck();
	//int count = 0;
	//while (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS) //fractal->time.value.GetTotalTime() < 2
	//{
	//	if (glfwGetKey(mainWindow, GLFW_KEY_R) == GLFW_PRESS)
	//	{
	//		// Synchronization is of no concern, as the nature of the buddhabrot is progressive; it doesn't change greatly after a single write
	//		glUseProgram(s.id);
	//		glUniform2f(sizeLocationFragment, (float)x, (float)y);
	//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//		glBindVertexArray(VAO);
	//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffHandle);
	//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffHandle);
	//	}
	//	fractal->time.value.PollTime();
	//	glfwSetWindowTitle(mainWindow, std::to_string(1 / fractal->time.value.GetDeltaTime()).c_str());

	//	count++;
	//	glUseProgram(c.id);
	//	glUniform1i(timeLocation, count);
	//	glDispatchCompute(x/16, y/16, 1);

	//	glfwSwapBuffers(mainWindow);
	//	glfwPollEvents();

	//}
	//GlErrorCheck();



	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffHandle);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffHandle);


	//glm::vec4* ptr;
	//ptr = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

	//Pixel* rgb = (Pixel*)malloc(x * y * sizeof(Pixel));
	//if (rgb == nullptr)
	//{
	//	return -1;
	//}


	//

	//const int brightArea = int(0.38046875 * x * x + 0.5 * y);
	//glm::vec3 brightness = glm::vec3(ptr[brightArea].x, ptr[brightArea].y, ptr[brightArea].z);
	//brightness = glm::max(brightness, glm::vec3(1.f));

	//for (size_t i = 0; i < (x-1); i++)
	//{
	//	for (size_t j = 0; j < y; j++)
	//	{
	//		size_t index = i * y + j;
	//		glm::vec3 c = glm::vec3(ptr[index].x, ptr[index].y, ptr[index].z);
	//		glm::vec4 col = glm::vec4((c / brightness), 1.0);


	//		Pixel* image = &rgb[index];
	//		image->red = (int)(col.x* 10);
	//		image->green = (int)(col.y* 10);
	//		image->blue = (int)(col.z*10);
	//		image->alpha = (int)(255);
	//	}


	//}

	//Image image(x, y, rgb);

	//try
	//{
	//	image.Save("C:/Users/Matis/source/repos/Mandelbulb/TestImage/test.png");
	//	DebugPrint("Successfully saved image \"test.png\"");
	//}
	//catch (const std::exception& e)
	//{
	//	DebugPrint("Error saving image: " + *e.what());
	//	return -1;
	//}

	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	//return -1;

	// render loop
	while (!glfwWindowShouldClose(mainWindow))
	{
		fractal->time.value.PollTime();
		fractal->explorationShader->SetUniform(fractal->time);

		fractal->Update();

#if _DEBUG
		// Set the window title to our fps
		std::string title = std::to_string(1 / fractal->time.value.GetDeltaTime());
		glfwSetWindowTitle(mainWindow, title.c_str());
#endif

		// render, we use ray marching inside the fragment shader
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
		fractal = (Fractal*)glfwGetWindowUserPointer(mainWindow);
		fractal->HandleKeyInput();
	}

	glDeleteVertexArrays(1, &vertexArray);
	glDeleteBuffers(1, &vertexIndices);
	delete fractal;
	glfwDestroyWindow(mainWindow);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}
