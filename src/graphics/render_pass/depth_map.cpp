#include <graphics/render_pass/depth_map.h>
#include <glm/gtc/matrix_transform.hpp>

DepthMapRenderPass::DepthMapRenderPass(
	SpotLight &light, Shader &shader, Framebuffer &fb, unsigned int width,
	unsigned int height
):
	light(light),
	shader(shader),
	fb(fb),
	width(width),
	height(height)
{}

glm::mat4
DepthMapRenderPass::light_view_projection()
{
	return light_vp;
}