#include <graphics/texture.h>

Texture::Texture(Image& tex_image, ColorLayout tex_layout):
	data(tex_image.data()),
	width(tex_image.width()),
	height(tex_image.height()),
	layout(tex_layout)
{}

Texture::Texture(
	unsigned char *data, unsigned int width, unsigned int height,
	ColorLayout layout
):
	data(data),
	width(width),
	height(height),
	layout(layout)
{}

void
Texture::load()
{
	GLenum format;
	switch (layout)
	{
		case layout_r:
			format = GL_RED;
			break;
		case layout_rgb:
			format = GL_RGB;
			break;
		case layout_rgba:
		default:
			format = GL_RGBA;
			break;
	}
	
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D, 0, format, width, height, 0, format,
		GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void
Texture::use(GLenum tex_unit)
{
	glActiveTexture(tex_unit);
	glBindTexture(GL_TEXTURE_2D, id);
}