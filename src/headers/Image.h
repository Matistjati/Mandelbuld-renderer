#pragma once
#ifndef IMAGE_H
#define IMAGE_H

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>

#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <glm.hpp>

#define byte unsigned char

struct Pixel
{
	byte r;
	byte g;
	byte b;
	byte a;

	Pixel(byte red, byte green, byte blue, byte alpha) : r(red), g(green), b(blue), a(alpha) {}

	byte& operator[](int index)
	{
		switch (index)
		{
		default: return r;
		case 0:  return r;
		case 1:  return g;
		case 2:  return b;
		case 3:  return a;
		}
	}

	static size_t Size() { return 4; }

	operator glm::ivec4() const
	{
		return glm::ivec4(r, g, b, a);
	}

	Pixel& operator=(const glm::ivec4 other)
	{
		//r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;
		return *this;
	}
};


class Image
{
public:
	Image(int width, int height, Pixel* pixels);
	~Image();
	void Save(const char *path);
	void FlipVertically();

	Pixel* pixels;
	int width;
	int height;
private:
	inline Pixel* PixelAt(int x, int y);
	inline void Swap(Pixel* a, Pixel* b);
};

#endif