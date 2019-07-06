#include <graphics/shader.h>
#include <iostream>
#include <file_loader.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

bool
is_compile_success(unsigned int shader_id)
{
	int success;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	return success != 0;
}

Shader::Shader(const char *vert_name, const char *frag_name)
{
	vert_filename = vert_name;
	frag_filename = frag_name;
}

void
Shader::try_create_and_link(void)
{
	// Load, compile and error check the vertex shader
	FileLoader vert_loader(vert_filename);
	std::string vert_shader_src = vert_loader.read();
	const char *vert_shader_csrc = vert_shader_src.c_str();
	unsigned int vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_shader_csrc, NULL);
	glCompileShader(vert_shader);
	if (!is_compile_success(vert_shader))
	{
		char info_log[512];
		glGetShaderInfoLog(vert_shader, 512, NULL, info_log);
		std::cout
			<< vert_filename << " compilation failed.\n" << info_log
			<< std::endl;
		throw ShaderCompileFailure();
	}
	
	// Load, compile and error check the fragment shader
	FileLoader frag_loader(frag_filename);
	std::string frag_shader_src = frag_loader.read();
	const char *frag_shader_csrc = frag_shader_src.c_str();
	unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_shader_csrc, NULL);
	glCompileShader(frag_shader);
	if (!is_compile_success(frag_shader))
	{
		char info_log[512];
		glGetShaderInfoLog(frag_shader, 512, NULL, info_log);
		std::cout
			<< frag_filename << " compilation failed.\n" << info_log
			<< std::endl;
		throw ShaderCompileFailure();
	}
	
	// Create and link shader program
	int success;
	char info_log[512];
	unsigned int program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, info_log);
		std::cout
			<< "Shader program linking failed.\n" << info_log
			<< std::endl;
		throw ShaderLinkFailure();
	}
	
	// Delete them once linked
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
	
	set_uniform("is_dir_light", 0);
	set_uniform("pt_light_count", 0);
	set_uniform("spot_light_count", 0);
	
	id = program;
}

void
Shader::set_uniform(const char *name, int value)
{
	glUniform1i(get_uniform_location(name), value);
}

void
Shader::set_uniform(const char *name, float value)
{
	glUniform1f(get_uniform_location(name), value);
}

void
Shader::set_uniform(const char *name, glm::mat4 matrix)
{
	int model_loc = get_uniform_location(name);
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void
Shader::set_uniform(const char *name, glm::vec3 vec)
{
	int model_loc = get_uniform_location(name);
	glUniform3f(model_loc, vec.x, vec.y, vec.z);
}

MaterialShader::MaterialShader(
	const char* vert_filename, const char* frag_filename
):
	Shader(vert_filename, frag_filename)
{};

void 
MaterialShader::set_dir_light(DirectionalLight& light)
{
	set_uniform("dir_light.direction", light.direction);
	set_uniform("dir_light.ambient", light.ambient);
	set_uniform("dir_light.diffuse", light.diffuse);
	set_uniform("dir_light.specular", light.specular);
	set_uniform("is_dir_light", 1);
}

void
MaterialShader::set_pt_light(PointLight& light)
{
	set_uniform("pt_lights[0].position", light.position);
	set_uniform("pt_lights[0].ambient", light.ambient);
	set_uniform("pt_lights[0].diffuse", light.diffuse);
	set_uniform("pt_lights[0].specular", light.specular);
	set_uniform("pt_lights[0].constant", light.att_constant);
	set_uniform("pt_lights[0].linear", light.att_linear);
	set_uniform("pt_lights[0].quadratic", light.att_quadratic);
	set_uniform("pt_light_count", 1);
}

void MaterialShader::set_spot_light(SpotLight& light)
{
	set_uniform("spot_lights[0].position", light.position);
	set_uniform("spot_lights[0].direction", light.direction);
	set_uniform("spot_lights[0].ambient", light.ambient);
	set_uniform("spot_lights[0].diffuse", light.diffuse);
	set_uniform("spot_lights[0].specular", light.specular);
	set_uniform("spot_lights[0].cut_off_cos", light.cut_off_cos);
	set_uniform(
		"spot_lights[0].outer_cut_off_cos", light.outer_cut_off_cos);
	set_uniform("spot_lights[0].constant", light.att_constant);
	set_uniform("spot_lights[0].linear", light.att_linear);
	set_uniform("spot_lights[0].quadratic", light.att_quadratic);
	set_uniform("spot_light_count", 1);
}

unsigned int
Shader::get_uniform_location(const char *uniform)
{
	return glGetUniformLocation(id, uniform);
}

void
Shader::use(void)
{
	glUseProgram(id);
}

