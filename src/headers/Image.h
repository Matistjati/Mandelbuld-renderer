#pragma once
#ifndef IMAGE_H
#define IMAGE_H

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>

#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct Pixel
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
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