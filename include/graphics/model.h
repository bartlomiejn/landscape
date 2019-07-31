#ifndef LANDSCAPE_MODEL_H
#define LANDSCAPE_MODEL_H

#include <glm/glm.hpp>
#include <graphics/mesh.h>
#include <graphics/shader.h>
#include <graphics/material.h>
#include <graphics/render_pass/context.h>

class Drawable
{
	/// Draw in a draw objects context.
	virtual void draw(DrawObjectsContext &ctx) = 0;
	
	/// Draw overriding the stored shader.
	virtual void draw(Shader &override_shader) = 0;
};

class Model : public Drawable
{
public:
	Mesh *mesh;
	Shader *shader;
	Material *material;
	glm::vec3 translation;
	glm::vec3 rotation_axis;
	float rotation_angle_rad;
	glm::vec3 scale;
	
	Model(
		Mesh *mesh,
		Shader *shader,
		Material *material,
		glm::vec3 translation,
		glm::vec3 rotation_axis = glm::vec3(0.0f, 0.1f, 0.0f),
		float rotation_angle_rad = 0.0f,
		glm::vec3 scale = glm::vec3(1.0f));
	
	glm::mat4 model_matrix() const;
	
	// Drawable
	
	/// Draw in a draw objects context.
	void draw(DrawObjectsContext &ctx) final;
	
	/// Draw overriding the stored shader. Used in depth map pass.
	void draw(Shader &override_shader) final;
};

#endif //LANDSCAPE_MODEL_H
