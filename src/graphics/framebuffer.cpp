#include <graphics/framebuffer.h>

Framebuffer::Framebuffer(Texture &depth_texture):
	depth_texture(depth_texture)
{}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &id);
}

void
Framebuffer::try_load()
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		depth_texture.id(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw FramebufferGenerationFailure();
}

void
Framebuffer::use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void
Framebuffer::set_draw(bool is_enabled)
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glDrawBuffer(GL_NONE);
}

void
Framebuffer::set_read(bool is_enabled)
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glReadBuffer(GL_NONE);
}

const Texture&
Framebuffer::get_depth_texture()
{
	return depth_texture;
}