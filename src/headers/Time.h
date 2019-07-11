#pragma once
#ifndef TIME_H
#define TIME_H

#include <glew.h>
#include <GLFW/glfw3.h>

class Time
{
public:
	Time() : lastTime(0), deltaTime(0) { firstTime = glfwGetTime(); }
	double deltaTime;
	double PollTime(double globalTime);
	double PollTime();
	double GetTotalTime() const;

private:
	double lastTime;
	double firstTime;
};

#endif