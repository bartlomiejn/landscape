#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <shader.h>

#define WIN_RES_X 800
#define WIN_RES_Y 600

float vertices[] = {
	0.5f,  0.5f, 0.0f, 	// Top right
	0.5f, -0.5f, 0.0f,  	// Bottom right
	-0.5f, -0.5f, 0.0f,  	// Bottom left
	-0.5f,  0.5f, 0.0f,   	// Top left
};

unsigned int indices[] = {
	0, 1, 3, // First triangle
	1, 2, 3, // Second triangle
};

float tex_coords[] = {
	0.0f, 0.0f,  // Lower-left corner
	1.0f, 0.0f,  // Lower-right corner
	0.5f, 1.0f   // Top-center
};

void
on_fb_resize(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void
handle_input(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int
main(void)
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
	
	// Compile and link triangle shader
	Shader rect_shader("glsl/vertex.glsl", "glsl/frag.glsl");
	try
	{
		rect_shader.try_create_and_link();
	}
	catch (ShaderCompilationFailure err)
	{
		glfwTerminate();
		return -1;
	}
	
	// Generate vertex array object which stores vertex attribute configs
	// and associated VBOs
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	// Generate vertex buffer object, copy vertices data, set to vertex
	// attribute pointer at 0 and enable
	unsigned int vbo;
	// Generate element buffer object that stores vertex indices
	unsigned int ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
		GL_STATIC_DRAW);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	// Perform rendering loop
	while (!glfwWindowShouldClose(window))
	{
		handle_input(window);
		
		// Calculate uniform
		float time = glfwGetTime();
		float time_val = (sin(time) / 2.0f) + 0.5f;
		
		// Clear the drawing buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Draw the triangle
		rect_shader.use();
		glUniform4f(
			rect_shader.get_uniform_location("appColor"),
			time_val, time_val, time_val, 1.0f);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}