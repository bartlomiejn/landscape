#ifndef LANDSCAPE_TEXTURE_H
#define LANDSCAPE_TEXTURE_H

#include <glad/glad.h>
#include <graphics/image.h>
#include <cstddef>

enum ColorLayout
{
	layout_depth16,
	layout_r,
	layout_rgb,
	layout_rgba,
};

enum FilterType
{
	filter_linear,
	filter_nearest,
};

class Texture
{
public:
	/// Generate an OpenGL texture from provided image data. `load` the
	/// texture before use!
	///
	/// \param data Image buffer to create a texture from.
	/// \param width Buffer width in bytes.
	/// \param height Buffer height in bytes.
	/// \param layout Color layout to use.
	Texture(
		unsigned char *data, unsigned int width, unsigned int height,
		ColorLayout layout, FilterType filter);
	
	/// Destroys the OpenGL texture on deallocation
	~Texture();
	
	/// Generates an OpenGL texture from provided image. `load` the
	/// texture before use!
	///
	/// \param image Image to create a texture from. Should be `load`ed
	/// before use.
	/// \param layout Color layout to use.
	Texture(Image* image, ColorLayout layout, FilterType filter);
	
	/// Retrieves the internal texture ID.
	unsigned int id() const;
	
	/// Loads the texture.
	void load();
	
	/// Uses the texture.
	///
	/// \param tex_unit Texture unit to use.
	void use(GLenum tex_unit) const;
private:
	unsigned int identifier;
	
	unsigned char *data;
	int width;
	int height;
	ColorLayout layout;
	FilterType filter;
	
	Image* image;
};

#endif //LANDSCAPE_TEXTURE_H
