#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <file_loader.h>

#define WIN_RES_X 800
#define WIN_RES_Y 600

float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f,  0.5f, 0.0f
};

void on_fb_resize(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void handle_input(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	// Setup GLFW window
	GLFWwindow* window = glfwCreateWindow(
		WIN_RES_X, WIN_RES_Y, "Landscape Engine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, on_fb_resize);
	
	// Load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD." << std::endl;
		return -1;
	}
	
	// Resize OpenGL viewport to initial window resolution
	on_fb_resize(nullptr, WIN_RES_X, WIN_RES_Y);
	
	// Generate vertex buffer object and copy vertices data
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// Load and compile vertex shader
	const char *vert_shader_name = "glsl/vertex.glsl";
	const char *vert_shader_src =
		FileLoader(vert_shader_name).read().c_str();
	unsigned int vert_shader;
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_shader_src, NULL);
	glCompileShader(vert_shader);
	
	// Check for compilation errors
	int success;
	char info_log[512];
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vert_shader, 512, NULL, info_log);
		std::cout << vert_shader_name << " compilation failed.\n"
			<< info_log << std::endl;
	}
	
	// Perform rendering loop
	while (!glfwWindowShouldClose(window))
	{
		handle_input(window);
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}