#ifndef LANDSCAPE_TEXTURE_H
#define LANDSCAPE_TEXTURE_H

#include <glad/glad.h>
#include <graphics/image.h>

enum ColorLayout
{
	layout_rgb,
	layout_rgba
};

class Texture
{
public:
	/// Generates an OpenGL texture from provided image.
	///
	/// \param image Image to create a texture from. Should be loaded before use.
	/// \param layout Color layout to use.
	Texture(Image image, ColorLayout layout);
	
	/// Uses the texture.
	///
	/// \param tex_unit Texture unit to use.
	void use(GLenum tex_unit);
private:
	unsigned int id;
};

#endif //LANDSCAPE_TEXTURE_H
