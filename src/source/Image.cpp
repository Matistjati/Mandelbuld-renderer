#include "headers/Image.h"
#include <stdexcept>
#include <iostream>

Image::Image(int width, int height, std::vector<Pixel> *pixels) : width(width), height(height), pixels(pixels), containsCopy(false)
{}

Image::Image(int width, int height, std::vector<glm::ivec4> pixels) : width(width), height(height), containsCopy(true)
{
	std::vector<Pixel> *data = new std::vector<Pixel>(width * height);

	size_t pixelCount = width * height;
	for (size_t i = 0; i < pixelCount; i++)
	{
		(*data)[i] = Pixel(pixels[i]);
	}
	this->pixels = data;
}

Image::~Image()
{
	if (containsCopy)
	{
		delete pixels;
	}
}


// You know i "borrowed" the code when there are comments in it :)
void Image::Save(const std::string path)
{
	FILE * fp;
	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	png_byte ** row_pointers = nullptr;


	// Trial and error (By someone else)
	const int pixel_size = 4;
	const int depth = 8;

	fp = fopen(path.c_str(), "wb");
	if (!fp)
	{
		std::cout << "Error saving image : bad file path : " << path << std::endl;
		throw std::invalid_argument("Error saving image: bad file path: " + path);
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (png_ptr == nullptr)
	{
		fclose(fp);
		throw std::invalid_argument("Could not create png pointer");
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == nullptr)
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

	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);
}

void Image::FlipVertically()
{
	try
	{
		for (int j = 0; j < width; j++)
		{
			for (int i = 0; i < height / 2; i++)
			{
				Swap(&(*pixels)[j + width * i], &(*pixels)[j + width * (height - i - 1)]);
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
	}
}

void Image::FlipAndSave(const std::string path)
{
	this->FlipVertically();
	this->Save(path);
}

inline Pixel* Image::PixelAt(int x, int y)
{
	return &(*pixels)[width * y + x];
}

inline void Image::Swap(Pixel * a, Pixel * b)
{
	Pixel temp = *a;
	*a = *b;
	*b = temp;
}
