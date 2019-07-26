#ifndef LANDSCAPE_IMAGE_H
#define LANDSCAPE_IMAGE_H

/// Sampling type enumeration
enum SamplingType
{
	sample_clamp, ///< Clamp to the image bounds.
};

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
	
	/// Samples the image at (x, y, channel). Throws an error on an invalid
	/// channel number.
	/// \param x X coordinate to sample.
	/// \param y Y coordinate to sample.
	/// \param channel Color channel to sample.
	/// \param sampling Sampling type to use.
	/// \return Byte at provided (x, y, channel).
	unsigned char try_sample(
		int x, int y, int channel,
		SamplingType sampling = sample_clamp) const;
	
	/// Returns the width of the image in pixels.
	int width() const;
	
	/// Returns the height of the image in pixels.
	int height() const;
	
	/// Returns the channel count for each pixel.
	int channel_count() const;
private:
	const char *img_filename;
	int img_width;
	int img_height;
	int img_channels;
};

class ImageLoadFailure {
public:
	ImageLoadFailure(const char *filename) : filename(filename) {};
	const char *filename;
};

class InvalidChannel {
public:
	InvalidChannel(const int channel) : channel(channel) {};
	const int channel;
};

#endif //LANDSCAPE_IMAGE_H
