#include "headers/Time.h"
#include "GLFW/glfw3.h"

double Time::PollTime(double globalTime)
{
	deltaTime = globalTime - lastTime;
	lastTime = globalTime;
	return deltaTime;
}

double Time::PollTime()
{
	double globalTime = glfwGetTime();
	deltaTime = globalTime - lastTime;
	lastTime = globalTime;
	return deltaTime;
}
