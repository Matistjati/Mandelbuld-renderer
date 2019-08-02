#include "headers/Time.h"
#include "GLFW/glfw3.h"
#include <iostream>

void Time::PollTime(double globalTime)
{
	deltaTime = globalTime - lastTime;
	lastTime = globalTime;
}

void Time::PollTime()
{
	double globalTime = glfwGetTime();
	deltaTime = globalTime - lastTime;
	lastTime = globalTime;
}

double Time::GetDeltaTime() const
{
	return deltaTime;
}

double Time::GetTotalTime() const
{
	if (timePaused)
	{
		return pausedTime;
	}
	else
	{
		return glfwGetTime() - firstTime;
	}
}

void Time::ToogleTimePause()
{
	if (timePaused) // Going to be onpaused
	{
		firstTime = glfwGetTime() - pausedTime;
		//std::cout << (glfwGetTime()-pausedTime)<<"        " << glfwGetTime()- (glfwGetTime() - pausedTime) << std::endl;
	}
	else //  Going to be paused
	{
		pausedTime = GetTotalTime();
		//std::cout << pausedTime << std::endl;
	}
	timePaused = !timePaused;
}

void Time::SetTotalTime(double time)
{
	firstTime = glfwGetTime() - time;
}
