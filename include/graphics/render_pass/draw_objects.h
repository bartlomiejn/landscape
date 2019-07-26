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
#include <graphics/render_pass/context.h>

class DrawObjectsRenderPass
{
public:
	SpotLight &light;
	MaterialShader &shader;
	Camera &camera;
	DepthMapRenderPass &depth_pass;
	unsigned int width;
	unsigned int height;
	float near_plane = 0.1f;
	float far_plane = 100.0f;
	
	DrawObjectsRenderPass(
		SpotLight &light,
		MaterialShader &shader,
		Camera &camera,
		DepthMapRenderPass &depth_pass,
		unsigned int width,
		unsigned int height
	) :
		light(light),
		shader(shader),
		camera(camera),
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
			255.0f);
		
		DrawObjectsContext ctx(
			view,
			projection,
			depth_pass.light_view_projection(),
			camera.position(),
			light,
			0,
			1,
			2,
			depth_pass.depth_texture());
		
		for (const auto &model : models)
			model->draw(ctx);
	}
};

#endif //LANDSCAPE_DRAW_OBJECTS_H
