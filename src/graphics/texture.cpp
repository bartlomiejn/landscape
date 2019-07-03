#include <graphics/texture.h>

Texture::Texture(Image image, ColorLayout layout)
{
	GLenum format;
	switch (layout)
	{
		case layout_rgb:
			format = GL_RGB;
			break;
		case layout_rgba:
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
		GL_TEXTURE_2D, 0, format, image.width(), image.height(), 0,
		format, GL_UNSIGNED_BYTE, image.data());
	glGenerateMipmap(GL_TEXTURE_2D);
}

void
Texture::use(GLenum tex_unit)
{
	glActiveTexture(tex_unit);
	glBindTexture(GL_TEXTURE_2D, id);
}