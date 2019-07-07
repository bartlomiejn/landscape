#include <graphics/model.h>
#include <glm/gtc/matrix_transform.hpp>

Model::Model(
	Mesh *mesh,
	Shader *shader,
	glm::vec3 translation,
	glm::vec3 rotation_axis,
	float rotation_rad_angle,
	glm::vec3 scale
):
	mesh(mesh),
	shader(shader),
	translation(translation),
	rotation_axis(rotation_axis),
	rotation_rad_angle(rotation_rad_angle),
	scale(scale)
{}

glm::mat4
Model::model_matrix()
{
	glm::mat4 model(1.0f);
	model = glm::translate(model, translation);
	model = glm::rotate(model, rotation_rad_angle, rotation_axis);
	model = glm::scale(model, scale);
	return model;
}

void
Model::draw()
{
	shader->set_uniform("model", model_matrix());
	mesh->use();
	mesh->draw();
}