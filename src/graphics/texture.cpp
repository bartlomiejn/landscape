#include <graphics/texture.h>

Texture::Texture(Image* tex_image, ColorLayout tex_layout, FilterType filter):
	image(tex_image),
	layout(tex_layout),
	filter(filter)
{}

Texture::Texture(
	unsigned char *data, unsigned int width, unsigned int height,
	ColorLayout layout, FilterType filter
):
	data(data),
	width(width),
	height(height),
	layout(layout),
	filter(filter)
{}

void
Texture::load()
{
	if (image != nullptr)
	{
		data = (unsigned char *)image->data();
		width = image->width();
		height = image->height();
	}
	
	GLenum gl_pix_layout;
	GLenum gl_internal_fmt;
	GLenum gl_pix_type = GL_UNSIGNED_BYTE;
	switch (layout)
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
	switch (filter)
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
Texture::id()
{
	return identifier;
}

void
Texture::use(GLenum tex_unit)
{
	glActiveTexture(tex_unit);
	glBindTexture(GL_TEXTURE_2D, identifier);
}