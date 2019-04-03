#pragma once
#ifndef TIME_H
#define TIME_H

class Time
{
public:
	double deltaTime;
	double PollTime(double globalTime);
	double PollTime();

private:
	double lastTime;
};

#endif