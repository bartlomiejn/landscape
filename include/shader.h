#ifndef LANDSCAPE_SHADER_H
#define LANDSCAPE_SHADER_H

class Shader
{
public:
	Shader(const char* vert_filename, const char* frag_filename);
	void try_create_and_link(void);
	unsigned int get_id(void);
	void set_app_color(float r, float g, float b, float a);
	unsigned int get_uniform_location(const char *uniform);
	void use(void);
private:
	unsigned int id;
	const char *vert_filename;
	const char *frag_filename;
};

class ShaderCompileFailure {};
class ShaderLinkFailure {};

#endif