#ifndef LANDSCAPE_DRAW_OBJECTS_H
#define LANDSCAPE_DRAW_OBJECTS_H

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <graphics/light.h>
#include <graphics/shader.h>
#include <graphics/framebuffer.h>
#include <graphics/model.h>
#include <graphics/model_group.h>
#include <graphics/camera.h>
#include <graphics/texture.h>
#include <graphics/render_pass/depth_map.h>
#include <graphics/render_pass/context.h>

class DrawObjectsRenderPass
{
public:
	DirectionalLight &light;
	MaterialShader &shader;
	Camera &camera;
	DepthMapRenderPass &depth_pass;
	unsigned int width;
	unsigned int height;
	float near_plane = 0.1f;
	float far_plane = 100.0f;
	
	DrawObjectsRenderPass(
		DirectionalLight &light,
		MaterialShader &shader,
		Camera &camera,
		DepthMapRenderPass &depth_pass,
		unsigned int width,
		unsigned int height
	);
	
	/// Prepares for drawing in default framebuffer. Use before any draw
	/// calls.
	void prepare();
	
	/// Draws objects in the default framebufer.
	///
	/// \tparam Container Container type
	/// \param models Container of pointers to Models
	void draw(std::vector<std::shared_ptr<Model>> &models);
	
	void draw(ModelGroup &models);
private:
	DrawObjectsContext create_context();
};

#endif //LANDSCAPE_DRAW_OBJECTS_H
