#ifndef LANDSCAPE_TEXTURE_H
#define LANDSCAPE_TEXTURE_H

#include <glad/glad.h>
#include <graphics/image.h>
#include <cstddef>

enum ColorLayout
{
	layout_r,
	layout_rgb,
	layout_rgba,
};

class Texture
{
public:
	/// Generates an OpenGL texture from provided image. `load` the
	/// texture before use!
	///
	/// \param image Image to create a texture from. Should be loaded before
	/// use.
	/// \param layout Color layout to use.
	Texture(Image* image, ColorLayout layout);
	
	/// Generate an OpenGL texture from provided image data. `load` the
	/// texture before use!
	///
	/// \param data Image buffer to create a texture from.
	/// \param width Buffer width in bytes.
	/// \param height Buffer height in bytes.
	/// \param layout Color layout to use.
	Texture(
		unsigned char *data, unsigned int width, unsigned int height,
		ColorLayout layout);
	
	/// Loads the texture.
	void load();
	
	/// Uses the texture.
	///
	/// \param tex_unit Texture unit to use.
	void use(GLenum tex_unit);
private:
	unsigned int id;
	Image* image;
	unsigned char *data;
	int width;
	int height;
	ColorLayout layout;
};

#endif //LANDSCAPE_TEXTURE_H
