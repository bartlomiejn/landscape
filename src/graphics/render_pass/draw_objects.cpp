#include <graphics/render_pass/draw_objects.h>

DrawObjectsRenderPass::DrawObjectsRenderPass(
	DirectionalLight &light,
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

void
DrawObjectsRenderPass::prepare()
{
	glViewport(0, 0, width, height);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);
}

void
DrawObjectsRenderPass::draw(std::vector<std::shared_ptr<Model>> &models)
{
	DrawObjectsContext ctx = create_context();
	for (const auto &model : models)
		model->draw(ctx);
}

void
DrawObjectsRenderPass::draw(ModelGroup &models)
{
	DrawObjectsContext ctx = create_context();
	models.draw(ctx);
}

DrawObjectsContext
DrawObjectsRenderPass::create_context()
{
	glm::mat4 view = camera.view_matrix();
	glm::mat4 projection = glm::perspective(
		glm::radians(camera.fov()),
		(float)width / (float)height,
		near_plane,
		far_plane);
	
	return DrawObjectsContext(
		view,
		projection,
		depth_pass.light_view_projection(),
		camera.position(),
		light,
		0,
		1,
		2,
		depth_pass.depth_texture());
}