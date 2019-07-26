#include <graphics/texture.h>
#include <iostream>

int color_layout_byte_size(ColorLayout layout)
{
	switch (layout)
	{
		// I assume 16 means the bits in OpenGL?
		case layout_depth16: 	return 2;
		case layout_r: 		return 1;
		case layout_rg: 	return 2;
		case layout_rgb: 	return 3;
		case layout_rgba: 	return 4;
		default:
			std::cout << __func__ <<  ": Unhandled type: "
				<< std::hex << (int)layout << std::endl;
			return -1;
	}
}

Texture::Texture(Image* tex_image, ColorLayout layout, FilterType filter):
	data(nullptr),
	is_data_owner(false),
	image(tex_image),
	width(tex_image->width()),
	height(tex_image->height()),
	tex_layout(layout),
	tex_filter(filter)
{}

Texture::Texture(
	unsigned char *data, unsigned int width, unsigned int height,
	ColorLayout layout, FilterType filter
):
	data(data),
	is_data_owner(false),
	width(width),
	height(height),
	tex_layout(layout),
	tex_filter(filter),
	image(nullptr)
{}

Texture::Texture(
	unsigned int width, unsigned int height, ColorLayout layout,
	FilterType filter
):
	data(new unsigned char[width * height * color_layout_byte_size(layout)]),
	is_data_owner(true),
	width(width),
	height(height),
	tex_layout(layout),
	tex_filter(filter),
	image(nullptr)
{}

Texture::~Texture()
{
	// If we're the owner of the data pointer, deallocate it once we're out
	if (is_data_owner)
		delete[] data;
	glDeleteTextures(1, &identifier);
}

void
Texture::load()
{
	if (image != nullptr)
	{
		data = image->data;
		width = image->width();
		height = image->height();
	}
	
	GLenum gl_pix_layout;
	GLenum gl_internal_fmt;
	GLenum gl_pix_type = GL_UNSIGNED_BYTE;
	switch (tex_layout)
	{
		case layout_depth16:
			gl_internal_fmt = GL_DEPTH_COMPONENT16;
			gl_pix_layout = GL_DEPTH_COMPONENT;
			gl_pix_type = GL_FLOAT;
			break;
		case layout_r:
			gl_internal_fmt = GL_RED;
			gl_pix_layout = GL_RED;
			break;
		case layout_rgb:
			gl_internal_fmt = GL_RGB;
			gl_pix_layout = GL_RGB;
			break;
		case layout_rgba:
		default:
			gl_internal_fmt = GL_RGBA;
			gl_pix_layout = GL_RGBA;
			break;
	}
	
	GLenum gl_filter;
	switch (tex_filter)
	{
		case filter_nearest:
			gl_filter = GL_NEAREST;
			break;
		case filter_linear:
		default:
			gl_filter = GL_LINEAR;
			break;
	}
	
	glGenTextures(1, &identifier);
	glBindTexture(GL_TEXTURE_2D, identifier);
	glTexImage2D(
		GL_TEXTURE_2D, 0, gl_internal_fmt, width, height, 0,
		gl_pix_layout, gl_pix_type, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
}

unsigned int
Texture::id() const
{
	return identifier;
}

void
Texture::use(GLenum tex_unit) const
{
	glActiveTexture(tex_unit);
	glBindTexture(GL_TEXTURE_2D, identifier);
}