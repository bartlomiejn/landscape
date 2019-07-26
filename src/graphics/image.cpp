#include <graphics/image.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Image::Image(const char *filename)
{
	img_filename = filename;
}

Image::Image(unsigned char *data, int width, int height, int channels) :
	data(data),
	img_width(width),
	img_height(height),
	img_channels(channels),
	img_filename(nullptr)
{}

void Image::try_load()
{
	if (!img_filename)
		throw ImageLoadFailure(nullptr);
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(
		img_filename, &img_width, &img_height, &img_channels, 0);
	if (!data)
		throw ImageLoadFailure(img_filename);
}

Image::~Image()
{
	if (img_filename)
		stbi_image_free(data);
}

int
Image::width() const
{
	return img_width;
}

int
Image::height() const
{
	return img_height;
}

int
Image::channels() const
{
	return img_channels;
}

ImageLoadFailure::ImageLoadFailure(const char *name)
{
	filename = name;
}