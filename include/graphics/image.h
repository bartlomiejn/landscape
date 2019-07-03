#ifndef LANDSCAPE_IMAGE_H
#define LANDSCAPE_IMAGE_H

class Image
{
public:
	Image(const char *filename);
	~Image();
	void try_load();
	int width();
	int height();
	int channels();
	const unsigned char *data();
private:
	const char *img_filename;
	int img_width;
	int img_height;
	int img_channels;
	unsigned char *img_data;
};

class ImageLoadFailure {
public:
	ImageLoadFailure(const char *filename);
	const char *filename;
};

#endif //LANDSCAPE_IMAGE_H
