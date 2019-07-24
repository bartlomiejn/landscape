#ifndef LANDSCAPE_DRAW_OBJECTS_H
#define LANDSCAPE_DRAW_OBJECTS_H

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <graphics/light.h>
#include <graphics/shader.h>
#include <graphics/framebuffer.h>
#include <graphics/model.h>
#include <graphics/camera.h>
#include <graphics/texture.h>
#include <graphics/render_pass/depth_map.h>

class DrawObjectsRenderPass
{
public:
	SpotLight &light;
	MaterialShader &shader;
	Camera &camera;
	Texture &cont_diff_tex;
	Texture &cont_spec_tex;
	Texture &wood_diff_tex;
	DepthMapRenderPass &depth_pass;
	unsigned int width;
	unsigned int height;
	float near_plane = 0.1f;
	float far_plane = 100.0f;
	
	DrawObjectsRenderPass(
		SpotLight &light,
		MaterialShader &shader,
		Camera &camera,
		Texture &cont_diff_tex,
		Texture &cont_spec_tex,
		Texture &wood_diff_tex,
		DepthMapRenderPass &depth_pass,
		unsigned int width,
		unsigned int height
	) :
		light(light),
		shader(shader),
		camera(camera),
		cont_diff_tex(cont_diff_tex),
		cont_spec_tex(cont_spec_tex),
		wood_diff_tex(wood_diff_tex),
		depth_pass(depth_pass),
		width(width),
		height(height)
	{}
	
	/// Draws a depth map for provided models.
	///
	/// \tparam Container Container type
	/// \param models Container of `std::reference_wrapper` of `Model`s
	template <typename Container>
	void
	draw(Container models)
	{
		glViewport(0, 0, width, height);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		glEnable(GL_DEPTH_TEST);
		
		// Generate view matrix
		glm::mat4 view = camera.view_matrix();
		
		// Generate perspective projection
		glm::mat4 projection = glm::perspective(
			glm::radians(camera.fov()),
			(float)width / (float)height,
			0.1f,
			100.0f);
		
		// Use the shader and configure its uniforms
		shader.use();
		shader.set_uniform("view", view);
		shader.set_uniform("projection", projection);
		shader.set_uniform(
			"light_space_matrix",
			depth_pass.light_view_projection());
		shader.set_uniform("view_pos", camera.position());
		shader.set_spot_light(light);
		shader.set_uniform("material.diffuse", 0);
		shader.set_uniform("material.specular", 1);
		shader.set_uniform("material.shininess", 32.0f);
		shader.set_uniform("shadow_map", 2);
		
		cont_diff_tex.use(GL_TEXTURE0);
		cont_spec_tex.use(GL_TEXTURE1);
		depth_pass.depth_texture().use(GL_TEXTURE2);
		
		for (const auto &model : models)
		{
			model.get()->draw();
		}
		
//		wood_diff_tex.use(GL_TEXTURE0);
//		wood_diff_tex.use(GL_TEXTURE1); // Use as specular as well
//		plane.draw();
	}
};

#endif //LANDSCAPE_DRAW_OBJECTS_H
