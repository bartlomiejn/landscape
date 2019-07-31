#include <numeric>
#include <graphics/model_group.h>

ModelGroup::ModelGroup() {}

ModelGroup::ModelGroup(std::vector<std::shared_ptr<Model>> models) :
	models(models)
{
	glm::mat4 model_mats[models.size()];
	std::transform(
		models.begin(),
		models.end(),
		model_mats,
		[](std::shared_ptr<Model> model) {
			return model->model_matrix();
		});
	buffer = GPU::Buffer(model_mats, sizeof(glm::mat4), models.size());
}

void
ModelGroup::push_back(std::shared_ptr<Model> model)
{
	models.push_back(model);
}

void
ModelGroup::remove(std::shared_ptr<Model> model)
{
	// Some kind of associative container would be faster here.
	for (int i = 0; i < models.size(); i++)
	{
		if (models[i] == model)
		{
			models.erase(models.begin() + i);
			break;
		}
	}
}

void
ModelGroup::draw(DrawObjectsContext &ctx)
{
	if (models.empty()
		|| models[0]->shader == nullptr
		|| models[0]->mesh == nullptr)
		return;
	
	// It will go wrong if models have different meshes/shaders, but for
	// now, this will suffice.
	Shader *shader = models[0]->shader;
	Mesh *mesh = models[0]->mesh;
	Material *mtl = models[0]->material;
	
	update_buffer();
	
	shader->use();
	
//	shader->set_uniform("model", models[0]->model_matrix());
	
	MaterialShader *mtl_shader = dynamic_cast<MaterialShader*>(shader);
	if (mtl_shader && mtl)
	{
		mtl_shader->set_uniform("view", ctx.view);
		mtl_shader->set_uniform("projection", ctx.projection);
		mtl_shader->set_uniform("light_space_matrix", ctx.light_vp);
		mtl_shader->set_uniform("view_pos", ctx.view_pos);
		mtl_shader->set_dir_light(ctx.light);
		mtl_shader->set_uniform(
			"material.shininess", mtl->shininess);
		mtl_shader->set_uniform("material.diffuse", (int)ctx.diff_slot);
		mtl_shader->set_uniform(
			"material.specular", (int)ctx.spec_slot);
		mtl_shader->set_uniform("shadow_map", (int)ctx.depthmap_slot);
		
		if (mtl->diffuse)
			mtl->diffuse->use(GL_TEXTURE0 + ctx.diff_slot);
		if (mtl->specular)
			mtl->specular->use(GL_TEXTURE0 + ctx.spec_slot);
		ctx.depthmap_tex.use(GL_TEXTURE0 + ctx.depthmap_slot);
	}
	
	mesh->use();
	mesh->draw();
}

void
ModelGroup::draw(Shader &override_shader)
{
	// TODO: Implementation
}

/// Updates the GPU buffer with model matrix content.
void
ModelGroup::update_buffer()
{
	// Update the buffer with the model matrices
	glm::mat4 model_mats[models.size()];
	transform(
		models.begin(),
		models.end(),
		model_mats,
		[](std::__1::shared_ptr<Model> model) {
			return model->model_matrix();
		});
	buffer.update(model_mats, sizeof(glm::mat4), models.size());
}
