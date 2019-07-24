#ifndef LANDSCAPE_FRAMEBUFFER_H
#define LANDSCAPE_FRAMEBUFFER_H

#include <graphics/texture.h>

class Framebuffer
{
public:
	Framebuffer(Texture &depth_texture);
	~Framebuffer();
	void try_load();
	void use();
	void set_draw(bool is_enabled);
	void set_read(bool is_enabled);
	const Texture& get_depth_texture();
private:
	Texture &depth_texture;
	unsigned int id;
};

class FramebufferGenerationFailure {};

#endif //LANDSCAPE_FRAMEBUFFER_H
