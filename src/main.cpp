#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <graphics/shader.h>
#include <graphics/image.h>
#include <graphics/camera.h>
#include <graphics/texture.h>
#include <graphics/light.h>

const unsigned int window_width = 800;
const unsigned int window_height = 600;

const unsigned int shadow_map_width = 1024;
const unsigned int shadow_map_height = 1024;

float cube_verts[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
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

float delta_time = 0.0f; /// Time between current frame and last frame
float last_frame = 0.0f; /// Time of last frame

float mouse_last_x = window_width / 2;
float mouse_last_y = window_height / 2;
float mouse_sensitivity = 0.05f;

float movement_speed = 2.5f;

Camera camera(
	glm::vec3(0.0f, 0.0f, 3.0f), // Position
	glm::vec3(0.0f, 1.0f, 0.0f), // World up vector
	0.0f, // Yaw
	0.0f, // Pitch
	45.0f); // FOV

DirectionalLight dir_light(
	glm::vec3(-0.2f, -1.0f, -0.3f), // Direction
	glm::vec3(0.05f, 0.05f, 0.05f), // Ambient
	glm::vec3(0.3f, 0.3f, 0.3f), 	// Diffuse
	glm::vec3(1.0f, 1.0f, 1.0f));	// Specular

PointLight pt_light(
	glm::vec3(1.2f, 1.0f, 2.0f), 	// Position
	glm::vec3(0.05f, 0.05f, 0.05f), // Ambient
	glm::vec3(0.5f, 0.5f, 0.5f), 	// Diffuse
	glm::vec3(1.0f, 1.0f, 1.0f));	// Specular

SpotLight spot_light(
	camera.position(),		// Position
	camera.front(),			// Direction
	glm::vec3(0.0f, 0.0f, 0.0f), 	// Ambient
	glm::vec3(0.5f, 0.5f, 0.5f), 	// Diffuse
	glm::vec3(1.0f, 1.0f, 1.0f),	// Specular
	glm::cos(glm::radians(12.5f)),	// Inner cut off
	glm::cos(glm::radians(17.5f))); // Outer cut off

MaterialShader material_shader("glsl/vertex.glsl", "glsl/material.glsl");
Shader light_shader("glsl/vertex.glsl", "glsl/light.glsl");
Shader shadow_map_shader(
	"glsl/shadowmap_vertex.glsl", "glsl/shadowmap_frag.glsl");

Image diff_img("assets/container2_diff.png");
Image spec_img("assets/container2_spec.png");

Texture container_diffuse(diff_img, layout_rgba);
Texture container_specular(spec_img, layout_rgba);

unsigned int vao; 		/// Cube vao
unsigned int depth_map_fbo; 	/// Shadow map fbo

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
shadow_map_pass()
{
	float near_plane = 1.0f;
	float far_plane = 7.5f;
	glm::mat4 light_projection =
		glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 light_view = glm::lookAt(
		glm::vec3(-2.0f, 4.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 light_space_matrix = light_projection * light_view;
	
	shadow_map_shader.use();
	shadow_map_shader.set_uniform("light_space_matrix", light_space_matrix);
	glViewport(0, 0, shadow_map_width, shadow_map_height);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void
draw_cubes(Shader &shader)
{
	glBindVertexArray(vao);
	for (unsigned int i = 0; i < 10; i++)
	{
		glm::mat4 model(1.0f);
		model = glm::translate(model, cube_positions[i]);
		float angle = 20.0f * i;
		float rotation =
			(float)glfwGetTime() * glm::radians(50.0f)
			+ glm::radians(angle);
		model = glm::rotate(
			model, rotation, glm::vec3(0.5f, 1.0f, 0.0f));
		shader.set_uniform("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void
draw_objects_pass()
{
	// Configure viewport to window settings
	glViewport(0, 0, window_width, window_height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// Clear the drawing buffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	// Generate view matrix
	glm::mat4 view = camera.view_matrix();
	
	// Generate perspective projection
	glm::mat4 projection = glm::perspective(
		glm::radians(camera.fov()),
		(float)window_width / (float)window_height,
		0.1f,
		100.0f);
	
	// Update spot light
	spot_light.position = camera.position();
	spot_light.direction = camera.front();
	
	// Use the shader and configure its uniforms
	material_shader.use();
	material_shader.set_uniform("view", view);
	material_shader.set_uniform("view_pos", camera.position());
	material_shader.set_uniform("projection", projection);
	material_shader.set_dir_light(dir_light);
	material_shader.set_pt_light(pt_light);
	material_shader.set_spot_light(spot_light);
	material_shader.set_uniform(
		"material.diffuse", 0);
	material_shader.set_uniform(
		"material.specular", 1);
	material_shader.set_uniform("material.shininess", 32.0f);
	
	// Bind textures
	container_diffuse.use(GL_TEXTURE0);
	container_specular.use(GL_TEXTURE1);
	
	draw_cubes(material_shader);
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
		window_width, window_height, "Landscape", NULL, NULL);
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
	on_fb_resize(nullptr, window_width, window_height);
	
	glEnable(GL_DEPTH_TEST);
	
	// Compile and link triangle shader
	try
	{
		material_shader.try_create_and_link();
		light_shader.try_create_and_link();
		shadow_map_shader.try_create_and_link();
		diff_img.try_load();
		spec_img.try_load();
		container_diffuse.load();
		container_specular.load();
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
	catch (ImageLoadFailure err)
	{
		std::cout << "Image loading failure: " << err.filename
			  << std::endl;
		glfwTerminate();
		return -1;
	}
	
	// Framebuffer object for rendering a depth map
	glGenFramebuffers(1, &depth_map_fbo);
	
	// Depth map
	unsigned int depth_map;
	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_map_width,
		shadow_map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// Bind the depth map to its FBO, disable draw/read for color data
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map,
		0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	// Bind the GLFW window FBO back
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// Generate vertex buffer object, copy vertices data, set to vertex
	// attribute pointer at 0 and enable
	unsigned int vbo;
	
	// Stride value for `vertices`
	size_t vert_stride = 8 * sizeof(float);
	
	// Generate vertex array object which stores vertex attribute configs
	// and associated VBOs
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);

	// Vertex position attribute
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, vert_stride, (void *)0);
	glEnableVertexAttribArray(0);
	// Vertex normal attribute
	glVertexAttribPointer(
		1, 3, GL_FLOAT, GL_FALSE, vert_stride,
		(void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Vertex texture coords attribute
	glVertexAttribPointer(
		2, 2, GL_FLOAT, GL_FALSE, vert_stride,
		(void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	// Perform rendering loop
	while (!glfwWindowShouldClose(window))
	{
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
		
		handle_keyboard_input(window);
		
		shadow_map_pass();
		draw_objects_pass();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glGenFramebuffers(1, &depth_map_fbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glfwTerminate();
	
	return 0;
}
