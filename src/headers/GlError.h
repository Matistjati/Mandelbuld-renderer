#pragma once
#ifndef GLERROR_H
#define GLERROR_H

#include "GLFW/glfw3.h"
#include <iostream>
#include <string>
#include <intrin.h>

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

#endif
