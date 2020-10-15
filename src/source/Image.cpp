#include "headers/Image.h"
#include "headers/Debug.h"
#include <stdexcept>
#include <iostream>

Image::Image(int width, int height, std::vector<Pixel> *pixels) : width(width), height(height), pixels(pixels), pixelsInitialized(false), hasAlpha(false)
{}

Image::Image(int width, int height, std::vector<glm::ivec4> pixels) : width(width), height(height), pixelsInitialized(true), hasAlpha(true)
{
	std::vector<Pixel> *data = new std::vector<Pixel>(width * height);

	size_t pixelCount = width * height;
	for (size_t i = 0; i < pixelCount; i++)
	{
		(*data)[i] = Pixel(pixels[i]);
	}
	this->pixels = data;
}

Image::Image(std::string filePath) : pixelsInitialized(true)
{
	png_bytep* row_pointers = NULL;

	FILE* fp = fopen(filePath.c_str(), "rb");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) abort();

	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	png_read_info(png, info);

	width = png_get_image_width(png, info);
	height = png_get_image_height(png, info);
	png_byte color_type = png_get_color_type(png, info);
	png_byte bit_depth = png_get_bit_depth(png, info);
	hasAlpha = (color_type & PNG_COLOR_MASK_ALPHA);

	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if (bit_depth == 16)
		png_set_strip_16(png);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	if (row_pointers) abort();

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for (int y = 0; y < height; y++) {
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
	}

	png_read_image(png, row_pointers);



	fclose(fp);

	png_destroy_read_struct(&png, &info, NULL);

	if (!hasAlpha)
	{
		//DebugPrint("There is currently no support for an alpha channel");
		//BreakIfDebug();
	}

	std::vector<Pixel>* data = new std::vector<Pixel>(width * height);
	for (int y = 0; y < height; y++)
	{
		png_bytep row = row_pointers[y];
		for (int x = 0; x < width; x++)
		{
			png_bytep px = &(row[x * 4]);
			(*data)[y * width + x] = Pixel(px[0], px[1], px[2], 255);
		}
	}


	this->pixels = data;
}

Image::~Image()
{
	if (pixelsInitialized)
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

void Image::Invert()
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel* pixel = PixelAt(x, y);
			pixel->r = 255 - pixel->r;
			pixel->g = 255 - pixel->g;
			pixel->b = 255 - pixel->b;
		}
	}
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
