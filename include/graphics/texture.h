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
	/// Pointer to texture storage if applicable.
	unsigned char *data;
	
	/// Generates an OpenGL texture from provided image. `load` the
	/// texture before use.
	/// \param image Image to create a texture from. Should be `load`ed
	/// before use.
	/// \param layout Color layout to use.
	/// \param filter Filtering type to use.
	Texture(Image* image, ColorLayout layout, FilterType filter);
	
	/// Generate a GPU texture from provided image data. `load` the texture
	/// before use.
	/// \param data Image buffer to create a texture from. The object does
	/// not take ownership of the data - dispose of it manually.
	/// \param width Buffer width in bytes.
	/// \param height Buffer height in bytes.
	/// \param layout Color layout to use.
	/// \param filter Filtering type to use.
	Texture(
		unsigned char *data, unsigned int width, unsigned int height,
		ColorLayout layout, FilterType filter);
	
	/// Generate a GPU texture with a data buffer of provided size.
	/// Acquires ownership of the buffer memory.
	/// \param width Buffer width in bytes.
	/// \param height Buffer height in bytes.
	/// \param layout Color layout to use.
	/// \param filter Filtering type to use.
	Texture(
		unsigned int width, unsigned int height, ColorLayout layout,
		FilterType filter);
	
	/// Destroys the texture. Will dispose of allocated buffer if
	/// ownership of it was acquired on creation.
	~Texture();
	
	/// Loads the texture to the GPU.
	void load();
	
	/// Retrieves the internal texture ID.
	unsigned int id() const;
	
	/// Uses the texture.
	/// \param tex_unit Texture unit to use.
	void use(GLenum tex_unit) const;
	
	/// Retrieves the texture pixel layout.
	ColorLayout layout() const;
	
	/// Retrieves the texture filtering type.
	FilterType filter() const;
private:
	unsigned int identifier;
	bool is_data_owner;
	int width;
	int height;
	ColorLayout tex_layout;
	FilterType tex_filter;
	Image* image;
};

#endif //LANDSCAPE_TEXTURE_H
