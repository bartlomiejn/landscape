#include <image.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Image::Image(const char *filename)
{
	img_filename = filename;
}

void Image::try_load()
{
	img_data = stbi_load(
		img_filename, &img_width, &img_height, &img_channels, 0);
	if (!img_data)
		throw ImageLoadFailure(img_filename);
}

Image::~Image()
{
	stbi_image_free(img_data);
}

int
Image::width()
{
	return img_width;
}

int
Image::height()
{
	return img_height;
}

int
Image::channels()
{
	return img_channels;
}

const unsigned char*
Image::data()
{
	return img_data;
}

ImageLoadFailure::ImageLoadFailure(const char *name)
{
	filename = name;
}