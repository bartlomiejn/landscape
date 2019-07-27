#ifndef LANDSCAPE_DEPTH_MAP_H
#define LANDSCAPE_DEPTH_MAP_H

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <graphics/light.h>
#include <graphics/shader.h>
#include <graphics/framebuffer.h>
#include <graphics/model.h>

class DepthMapRenderPass
{
public:
	DirectionalLight &light;
	Shader &shader;
	Framebuffer &fb;
	unsigned int width;
	unsigned int height;
	float near_plane = 0.1f;
	float far_plane = 100.0f;
	
	/// Creates an instance of a depth map calculation render pass for the
	/// provided light source.
	///
	/// \param light Light source to generate a depth map for.
	/// \param shader Shader for the objects drawn.
	/// \param fb Framebuffer to draw the depth map into.
	/// \param width Depth map width.
	/// \param height Depth map height.
	DepthMapRenderPass(
		DirectionalLight &light, Shader &shader, Framebuffer &fb,
		unsigned int width, unsigned int height
	):
		light(light),
		shader(shader),
		fb(fb),
		width(width),
		height(height)
	{};
	
	/// Draws a depth map for provided models.
	///
	/// \tparam Container Container type
	/// \param models Container of pointers to `Model`s
	template <typename Container>
	void
	draw(Container models)
	{
		glViewport(0, 0, width, height);
		
		fb.use();
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		glEnable(GL_DEPTH_TEST);
		
		glm::mat4 light_proj = glm::ortho(
			-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
		glm::mat4 light_view = glm::lookAt(
			-light.direction + light.look_at,
			light.look_at,
			glm::vec3(0.0f, 1.0f, 0.0f));
		light_vp = light_proj * light_view;
		
		shader.use();
		shader.set_uniform("light_space_matrix", light_vp);

		for (const auto &model : models)
			model->draw(shader);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	/// Returns the light source's view-projection matrix, which is computed
	/// on each `draw` call. Used for shadow calculation in the material
	/// shader.
	///
	/// \return View-projection matrix for the light source.
	glm::mat4
	light_view_projection()
	{
		return light_vp;
	}
	
	const Texture&
	depth_texture()
	{
		return fb.get_depth_texture();
	}
private:
	glm::mat4 light_vp;
};

#endif //LANDSCAPE_DEPTH_MAP_H
