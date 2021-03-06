#ifndef LANDSCAPE_SHADER_H
#define LANDSCAPE_SHADER_H

#include <glm/glm.hpp>
#include <graphics/light.h>
#include <graphics/material.h>

class Shader
{
public:
	Shader(const char* vert_filename, const char* frag_filename);
	virtual ~Shader() = default;
	void try_create_and_link(void);
	void set_uniform(const char *name, int value);
	void set_uniform(const char *name, float value);
	void set_uniform(const char *name, glm::mat4 matrix);
	void set_uniform(const char *name, glm::vec3 vec);
	void use(void) const;
	unsigned int get_uniform_location(const char *uniform) const;
private:
	unsigned int id;
	const char *vert_filename;
	const char *frag_filename;
};

class MaterialShader: public Shader
{
public:
	explicit MaterialShader(const char* vert_filename);
	void set_dir_light(DirectionalLight& light);
	void set_pt_light(PointLight& light);
	void set_spot_light(SpotLight& light);
};

class ShaderCompileFailure {};
class ShaderLinkFailure {};

#endif