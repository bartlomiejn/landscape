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
	SpotLight &light;
	Shader &shader;
	Framebuffer &fb;
	unsigned int width;
	unsigned int height;
	float near_plane = 0.1f;
	float far_plane = 100.0f;
	
	DepthMapRenderPass(
		SpotLight &light, Shader &shader, Framebuffer &fb,
		unsigned int width, unsigned int height);
	
	/// Draws a depth map for provided models.
	///
	/// \tparam Container Container type
	/// \param models Container of `std::reference_wrapper` of `Model`s
	template <typename Container> void draw(Container models)
	{
		glViewport(0, 0, width, height);
		
		fb.use();
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		glEnable(GL_DEPTH_TEST);
		
		glm::mat4 light_proj =
			glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 light_view = glm::lookAt(
			light.position,
			light.direction,
			glm::vec3(0.0f, 1.0f, 0.0f));
		light_vp = light_proj * light_view;
		
		shader.use();
		shader.set_uniform("light_space_matrix", light_vp);
		
		Shader *temp;
		for (const std::shared_ptr<Model> &model : models)
		{
			temp = model->shader;
			model->shader = &shader;
			model->draw();
			model->shader = temp;
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	glm::mat4 light_view_projection();
private:
	glm::mat4 light_vp;
};

#endif //LANDSCAPE_DEPTH_MAP_H