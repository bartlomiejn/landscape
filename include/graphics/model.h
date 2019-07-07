#ifndef LANDSCAPE_MODEL_H
#define LANDSCAPE_MODEL_H

#include <graphics/mesh.h>
#include <graphics/shader.h>
#include <glm/glm.hpp>

class Model
{
public:
	Mesh *mesh;
	Shader *shader;
	glm::vec3 translation;
	glm::vec3 rotation_axis;
	float rotation_rad_angle;
	glm::vec3 scale;
	
	Model(
		Mesh *mesh,
		Shader *shader,
		glm::vec3 translation,
		glm::vec3 rotation_axis = glm::vec3(0.0f, 0.1f, 0.0f),
		float rotation_rad_angle = 0.0f,
		glm::vec3 scale = glm::vec3(1.0f));
	
	glm::mat4 model_matrix();
	void draw();
};

#endif //LANDSCAPE_MODEL_H
