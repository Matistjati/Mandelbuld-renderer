#pragma once
#ifndef TIME_H
#define TIME_H

#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>

class Time
{
public:
	Time() : lastTime(0), deltaTime(0), timePaused(false), pausedTime(0) { firstTime = glfwGetTime(); }

	void PollTime(double globalTime);
	void PollTime();

	double GetDeltaTime() const;
	double GetTotalTime() const;
	void ToogleTimePause();
	void SetTotalTime(double time);

private:
	double deltaTime;
	double lastTime;
	double firstTime;
	double pausedTime;
	bool timePaused;
};

#endif