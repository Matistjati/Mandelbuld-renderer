#include "headers/Image.h"
#include <stdexcept>
#include <iostream>

Image::Image(int width, int height, Pixel * pixels) : width(width), height(height)
{
	if (pixels == nullptr)
	{
		throw std::invalid_argument("pixels is null");
	}
	this->pixels = pixels;
}

Image::~Image()
{
	free(pixels);
}

// You know i stole the code when there are comments in it :)
void Image::Save(const char * path)
{
	FILE * fp;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_byte ** row_pointers = NULL;


	// Trial and error (By someone else)
	const int pixel_size = 4;
	const int depth = 8;

	fp = fopen(path, "wb");
	if (!(fp))
	{
		std::cout << "Error saving image: bad file path: \"" << path << "\"" << std::endl;
		return;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		fclose(fp);
		throw std::invalid_argument("Could not create png pointer");
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		throw std::invalid_argument("Could not create info pointer");
	}

	// Set up error handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		throw std::invalid_argument("Could not set up error handling");
	}

	// Set image attributes

	png_set_IHDR(png_ptr,
		info_ptr,
		width,
		height,
		depth,
		PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);

	// Initialize rows of PNG.

	row_pointers = (png_byte**)png_malloc(png_ptr, height * sizeof(png_byte *));
	for (int y = 0; y < height; ++y)
	{
		png_byte *row = (png_byte*)png_malloc(png_ptr, sizeof(uint8_t) * width * pixel_size);

		row_pointers[y] = row;
		for (int x = 0; x < width; ++x)
		{
			Pixel* pixel = PixelAt(x, y);
			*row++ = pixel->r;
			*row++ = pixel->g;
			*row++ = pixel->b;
			*row++ = pixel->a;
		}
	}

	// Write image data to the file
	
	png_init_io(png_ptr, fp);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	// Free the allocated memory
	for (int y = 0; y < height; y++)
	{
		png_free(png_ptr, row_pointers[y]);
	}
	png_free(png_ptr, row_pointers);

	fclose(fp);
}

void Image::FlipVertically()
{
	for (int j = 0; j < width; j++)
	{
		for (int i = 0; i < height / 2; i++)
		{
			Swap(&pixels[j + width * i], &pixels[j + width * (height - i - 1)]);
		}
	}
}

inline Pixel* Image::PixelAt(int x, int y)
{
	return pixels + width * y + x;
}

inline void Image::Swap(Pixel * a, Pixel * b)
{
	Pixel temp = *a;
	*a = *b;
	*b = temp;
}
