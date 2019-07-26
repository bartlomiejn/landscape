#ifndef LANDSCAPE_IMAGE_H
#define LANDSCAPE_IMAGE_H

class Image
{
public:
	unsigned char *data; ///< Raw pointer to the image data. Do not alter
			     ///< unless you know what you're doing.
	
	/// Loads an image from the provided filename.
	/// \param filename Filename of the image to load.
	Image(const char *filename);
	
	/// Creates a new image from provided parameters.
	/// \param data Data buffer. Does not take ownership of the data buffer.
	/// Dispose of it manually.
	/// \param width Width in pixels.
	/// \param height Height in pixels.
	/// \param channels Channel count for each pixel.
	Image(unsigned char *data, int width, int height, int channels);
	
	/// Destroys an image.
	~Image();
	
	/// Attempts to load the image from the provided filename. Can throw an
	/// exception.
	void try_load();
	
	/// Returns the width of the image in pixels.
	int width() const;
	
	/// Returns the height of the image in pixels.
	int height() const;
	
	/// Returns the channel count for each pixel.
	int channels() const;
private:
	const char *img_filename;
	int img_width;
	int img_height;
	int img_channels;
};

class ImageLoadFailure {
public:
	ImageLoadFailure(const char *filename);
	const char *filename;
};

#endif //LANDSCAPE_IMAGE_H
