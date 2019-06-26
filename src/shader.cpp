#include <shader.h>
#include <iostream>
#include <file_loader.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
	const char *vert_shader_src =
		FileLoader(vert_filename).read().c_str();
	unsigned int vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_shader_src, NULL);
	glCompileShader(vert_shader);
	if (!is_compile_success(vert_shader))
	{
		char info_log[512];
		glGetShaderInfoLog(vert_shader, 512, NULL, info_log);
		std::cout << vert_filename << " compilation failed.\n"
			  << info_log << std::endl;
		throw ShaderCompilationFailure();
	}
	
	// Load, compile and error check the fragment shader
	const char *frag_shader_src =
		FileLoader(frag_filename).read().c_str();
	unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_shader_src, NULL);
	glCompileShader(frag_shader);
	if (!is_compile_success(frag_shader))
	{
		char info_log[512];
		glGetShaderInfoLog(frag_shader, 512, NULL, info_log);
		std::cout << frag_filename << " compilation failed.\n"
			  << info_log << std::endl;
		throw ShaderCompilationFailure();
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
		std::cout << "Shader program linking failed.\n" << info_log
			  << std::endl;
	}
	
	// Delete them once linked
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
	
	id = program;
}

unsigned int
Shader::get_uniform_location(const char *uniform)
{
	return glGetUniformLocation(id, uniform);
}

unsigned int
Shader::get_id(void)
{
	return id;
}

void
Shader::use(void)
{
	glUseProgram(id);
}

