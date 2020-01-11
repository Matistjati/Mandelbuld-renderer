#pragma once
#ifndef GLERROR_H
#define GLERROR_H

#include <iostream>
#include <string>
#include <intrin.h>
#include <string>
#include "GLFW/glfw3.h"
#include <glad/glad.h>

inline void GlErrorCheck()
{
#if _DEBUG
	int err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "Opengl error: " << std::to_string(err) << std::endl;
		__debugbreak();
	}
#endif
}

inline void DebugPrint(std::string content)
{
#if _DEBUG
	std::cout << content << std::endl;
#endif
}

inline void BreakIfDebug()
{
#if _DEBUG
		__debugbreak();
#endif
}

#endif
