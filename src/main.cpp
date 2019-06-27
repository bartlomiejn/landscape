#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <shader.h>
#include <image.h>

#define WIN_RES_X 800
#define WIN_RES_Y 600

float vertices[] = {
	// positions         // colors          // texture coords
	0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void
do_exit_cleanup(uint vao, uint vbo, uint ebo)
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glfwTerminate();
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
	catch (ShaderCompileFailure err)
	{
		glfwTerminate();
		return -1;
	}
	catch (ShaderLinkFailure err)
	{
		glfwTerminate();
		return -1;
	}
	
	// Generate vertex array object which stores vertex attribute configs
	// and associated VBOs
	unsigned int vao;
	// Generate vertex buffer object, copy vertices data, set to vertex
	// attribute pointer at 0 and enable
	unsigned int vbo;
	// Generate element buffer object that stores vertex indices
	unsigned int ebo;
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
		GL_STATIC_DRAW);
	
	size_t vert_stride = 8 * sizeof(float); // Stride value for `vertices`
	
	// Position attribute
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, vert_stride, (void *)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(
		1, 3, GL_FLOAT, GL_FALSE, vert_stride,
		(void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture coordinates attribute
	glVertexAttribPointer(
		2, 2, GL_FLOAT, GL_FALSE, vert_stride,
		(void *)(6 * sizeof(float)));;
	glEnableVertexAttribArray(2);
	
	// Load images
	Image image("assets/container.jpg");
	Image image2("assets/awesomeface.png");
	try
	{
		image.try_load();
		image2.try_load();
	}
	catch (ImageLoadFailure failure)
	{
		std::cout
			<< "Failed to load texture." << failure.filename
			<< std::endl;
		glfwTerminate();
		return -1;
	}
	
	// Generate textures
	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0,
		GL_RGB, GL_UNSIGNED_BYTE, image.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	unsigned int tex2;
	glGenTextures(1, &tex2);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, image2.width(), image2.height(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image2.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	
	// Perform rendering loop
	while (!glfwWindowShouldClose(window))
	{
		handle_input(window);
		
		// Clear the drawing buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Set textures for the rect shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 1);
		
		// Use the shader and set the sampler uniforms
		rect_shader.use();
		rect_shader.set_uniform("texture1", 0);
		rect_shader.set_uniform("texture2", 1);
		
		// Draw the rect
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	do_exit_cleanup(vao, vbo, ebo);
	
	return 0;
}