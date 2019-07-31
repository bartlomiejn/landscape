#include <glm/gtc/matrix_transform.hpp>
#include <graphics/model.h>


Model::Model(
	Mesh *mesh,
	Shader *shader,
	Material *material,
	glm::vec3 translation,
	glm::vec3 rotation_axis,
	float rotation_angle_rad,
	glm::vec3 scale
):
	mesh(mesh),
	shader(shader),
	material(material),
	translation(translation),
	rotation_axis(rotation_axis),
	rotation_angle_rad(rotation_angle_rad),
	scale(scale)
{}

glm::mat4
Model::model_matrix() const
{
	glm::mat4 model(1.0f);
	model = glm::translate(model, translation);
	model = glm::rotate(model, rotation_angle_rad, rotation_axis);
	model = glm::scale(model, scale);
	return model;
}

void
Model::draw(DrawObjectsContext &ctx)
{
	if (shader == nullptr || mesh == nullptr)
		return;
	
	shader->use();
	shader->set_uniform("model", model_matrix());
	
	MaterialShader *mtl_shader = dynamic_cast<MaterialShader*>(shader);
	if (mtl_shader && material)
	{
		mtl_shader->set_uniform("view", ctx.view);
		mtl_shader->set_uniform("projection", ctx.projection);
		mtl_shader->set_uniform("light_space_matrix", ctx.light_vp);
		mtl_shader->set_uniform("view_pos", ctx.view_pos);
		mtl_shader->set_dir_light(ctx.light);
		mtl_shader->set_uniform(
			"material.shininess", material->shininess);
		mtl_shader->set_uniform("material.diffuse", (int)ctx.diff_slot);
		mtl_shader->set_uniform(
			"material.specular", (int)ctx.spec_slot);
		mtl_shader->set_uniform("shadow_map", (int)ctx.depthmap_slot);
		
		if (material->diffuse)
			material->diffuse->use(GL_TEXTURE0 + ctx.diff_slot);
		if (material->specular)
			material->specular->use(GL_TEXTURE0 + ctx.spec_slot);
		ctx.depthmap_tex.use(GL_TEXTURE0 + ctx.depthmap_slot);
	}
	
	mesh->use();
	mesh->draw();
}

void
Model::draw(Shader &override_shader)
{
	if (mesh == nullptr)
		return;
	override_shader.set_uniform("model", model_matrix());
	mesh->use();
	mesh->draw();
}