#ifndef LANDSCAPE_SHADER_H
#define LANDSCAPE_SHADER_H

#include <glm/glm.hpp>

class Shader
{
public:
	Shader(const char* vert_filename, const char* frag_filename);
	
	void try_create_and_link(void);
	void use(void);
	void set_uniform(const char *name, int value);
	void set_uniform(const char *name, float value);
	void set_uniform(const char *name, glm::mat4 matrix);
	void set_uniform(const char *name, glm::vec3 vec);
	unsigned int get_uniform_location(const char *uniform);
private:
	unsigned int id;
	const char *vert_filename;
	const char *frag_filename;
};

class ShaderCompileFailure {};
class ShaderLinkFailure {};

#endif