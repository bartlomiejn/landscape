#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>
#include <image.h>
#include <camera.h>

#define WIN_RES_X 800
#define WIN_RES_Y 600

float cube_verts[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

glm::vec3 cube_positions[] = {
	glm::vec3( 0.0f,  0.0f,  0.0f),
	glm::vec3( 2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3( 2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3( 1.3f, -2.0f, -2.5f),
	glm::vec3( 1.5f,  2.0f, -2.5f),
	glm::vec3( 1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
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

float delta_time = 0.0f; /// Time between current frame and last frame
float last_frame = 0.0f; /// Time of last frame

float mouse_last_x = WIN_RES_X / 2;
float mouse_last_y = WIN_RES_Y / 2;
float mouse_sensitivity = 0.05f;

float movement_speed = 2.5f;

Camera camera(
	glm::vec3(0.0f, 0.0f, 3.0f), // Position
	glm::vec3(0.0f, 1.0f, 0.0f), // World up vector
	0.0f, // Yaw
	0.0f, // Pitch
	45.0f); // FOV

void
on_fb_resize(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void
handle_mouse_movement(GLFWwindow *window, double x_pos, double y_pos)
{
	static bool is_first_mouse = true;
	if (is_first_mouse)
	{
		mouse_last_x = x_pos;
		mouse_last_y = y_pos;
		is_first_mouse = false;
	}
	
	float x_offset = x_pos - mouse_last_x;
	float y_offset = mouse_last_y - y_pos;
	mouse_last_x = x_pos;
	mouse_last_y = y_pos;
	
	x_offset *= mouse_sensitivity;
	y_offset *= mouse_sensitivity;
	
	camera.rotate(x_offset, y_offset);
}

void
handle_mouse_scroll(GLFWwindow *window, double x_offset, double y_offset)
{
	camera.zoom_in(y_offset);
}

void
handle_keyboard_input(GLFWwindow *window)
{
	float per_frame_speed = movement_speed * delta_time;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.move(direction_forward, per_frame_speed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.move(direction_backward, per_frame_speed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.move(direction_left, per_frame_speed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.move(direction_right, per_frame_speed);
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
	glfwSetCursorPosCallback(window, handle_mouse_movement);
	glfwSetScrollCallback(window, handle_mouse_scroll);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
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
		GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
		GL_STATIC_DRAW);
	
	size_t vert_stride = 5 * sizeof(float); // Stride value for `vertices`
	
	// Position attribute
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, vert_stride, (void *)0);
	glEnableVertexAttribArray(0);
	// Texture coordinates attribute
	glVertexAttribPointer(
		1, 2, GL_FLOAT, GL_FALSE, vert_stride,
		(void *)(3 * sizeof(float)));;
	glEnableVertexAttribArray(1);
	
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
	
	glEnable(GL_DEPTH_TEST);
	
	// Apply initial rotation to model based on elapsed time
	glm::mat4 model = glm::rotate(
		glm::mat4(1.0f), (float)glfwGetTime() * glm::radians(50.0f),
		glm::vec3(0.5f, 1.0f, 0.0f));
	
	// Generate perspective projection
	glm::mat4 projection = glm::perspective(
		glm::radians(camera.fov()), (float)WIN_RES_X / (float)WIN_RES_Y,
		0.1f, 100.0f);

	// Perform rendering loop
	while (!glfwWindowShouldClose(window))
	{
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
		
		handle_keyboard_input(window);
		
		glm::mat4 view = camera.view_matrix();
		
		// Clear the drawing buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		// Set textures for the shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex2);
		
		// Use the shader and set the texture & transformation uniforms
		rect_shader.use();
		rect_shader.set_uniform("texture1", 0);
		rect_shader.set_uniform("texture2", 1);
		rect_shader.set_uniform("view", view);
		rect_shader.set_uniform("projection", projection);
		
		// Draw the models
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cube_positions[i]);
			float angle = 20.0f * i;
			float rotation =
				(float)glfwGetTime() * glm::radians(50.0f)
				+ glm::radians(angle);
			model = glm::rotate(
				model, rotation, glm::vec3(0.5f, 1.0f, 0.0f));
			rect_shader.set_uniform("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	do_exit_cleanup(vao, vbo, ebo);
	
	return 0;
}
