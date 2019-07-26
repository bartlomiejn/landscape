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

unsigned char Image::try_sample(
	int x, int y, int channel, SamplingType sampling
) const {
	if (channel >= channel_count())
		throw InvalidChannel(channel);
	if (x > width() || x < 0)
		switch (sampling)
		{
			case sample_clamp:
			default:
				if (x < 0) x = 0;
				else 	   x = width();
		}
	if (y > height() || y < 0)
		switch (sampling)
		{
			case sample_clamp:
			default:
				if (y < 0) y = 0;
				else 	   y = height();
		}
	return data[
		y * height() * channel_count() + x * channel_count() + channel];
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
Image::channel_count() const
{
	return img_channels;
}
