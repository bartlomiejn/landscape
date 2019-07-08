#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <graphics/shader.h>
#include <graphics/image.h>
#include <graphics/camera.h>
#include <graphics/texture.h>
#include <graphics/light.h>
#include <graphics/mesh.h>
#include <graphics/model.h>
#include <graphics/primitives/cube.h>
#include <graphics/primitives/plane.h>

const unsigned int window_width = 1280;
const unsigned int window_height = 720;
const unsigned int shadow_map_width = 1024;
const unsigned int shadow_map_height = 1024;

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
	glm::vec3(0.0f, 0.0f, 3.0f), 	// Position
	glm::vec3(0.0f, 1.0f, 0.0f), 	// World up vector
	0.0f, 				// Yaw
	0.0f, 				// Pitch
	45.0f); 			// FOV
	
// TODO: Shading is based on a point light, but shadows are directional
SpotLight shadow_map_spot_light(
	glm::vec3(-4.0f, 10.0f, -2.0f),	// Position
	glm::vec3(0.0f, 0.0f, 0.1f),	// Direction
	glm::vec3(0.3f, 0.3f, 0.3f), 	// Ambient
	glm::vec3(0.5f, 0.5f, 0.5f), 	// Diffuse
	glm::vec3(1.0f, 1.0f, 1.0f),	// Specular
	glm::cos(glm::radians(12.5f)),	// Inner cut off
	glm::cos(glm::radians(17.5f))); // Outer cut off
	
MaterialShader material_shader("glsl/vertex.glsl", "glsl/material.glsl");
Shader depth_map_shader(
	"glsl/shadowmap_vertex.glsl", "glsl/shadowmap_frag.glsl");
Shader depth_debug_shader(
	"glsl/shadowmap_vertex_quad.glsl", "glsl/shadowmap_frag_quad.glsl");
Shader white_shader("glsl/vertex.glsl", "glsl/white.glsl");

Image cont_diff_img("assets/container2_diff.png");
Image cont_spec_img("assets/container2_spec.png");
Image wood_diff_img("assets/wood_diff.png");

Texture cont_diff_tex(cont_diff_img, layout_rgba);
Texture cont_spec_tex(cont_spec_img, layout_rgba);
Texture wood_diff_tex(wood_diff_img, layout_rgba);

CubeMesh cube_mesh;
PlaneMesh plane_mesh;

std::vector<Model> cubes;
Model plane(&plane_mesh, &material_shader, glm::vec3(0.0f, -2.4f, 0.0f));

// Depth map

// TODO: Extract to a render pass object
glm::mat4 light_view_projection;	/// Light depth map VP matrix
unsigned int depth_map_fbo; 		/// Light depth map fbo
unsigned int depth_map_tex;		/// Light depth map texture buffer
float near_plane = 0.1f;
float far_plane = 100.0f;

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
	glm::mat4 light_proj =
		glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 light_view = glm::lookAt(
		shadow_map_spot_light.position,
		shadow_map_spot_light.direction,
		glm::vec3(0.0f, 1.0f, 0.0f));
	light_view_projection = light_proj * light_view;
	
	depth_map_shader.use();
	depth_map_shader.set_uniform(
		"light_space_matrix", light_view_projection);
	
	glViewport(0, 0, shadow_map_width, shadow_map_height);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	Shader *temp;
	for (unsigned int i = 0; i < 10; i++)
	{
		temp = cubes[i].shader;
		cubes[i].shader = &depth_map_shader;
		cubes[i].draw();
		cubes[i].shader = temp;
	}
	temp = plane.shader;
	plane.shader = &depth_map_shader;
	plane.draw();
	plane.shader = temp;
}

void
draw_objects_pass()
{
	// TODO: On retina-like displays, the viewport should be multiplied by 2
	// TODO: Find out how to check if the display is @2x
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
	
	// Use the shader and configure its uniforms
	material_shader.use();
	material_shader.set_uniform("view", view);
	material_shader.set_uniform("projection", projection);
	material_shader.set_uniform("light_space_matrix", light_view_projection);
	material_shader.set_uniform("view_pos", camera.position());
	material_shader.set_spot_light(shadow_map_spot_light);
	material_shader.set_uniform("material.diffuse", 0);
	material_shader.set_uniform("material.specular", 1);
	material_shader.set_uniform("material.shininess", 32.0f);
	material_shader.set_uniform("shadow_map", 2);
	
	cont_diff_tex.use(GL_TEXTURE0);
	cont_spec_tex.use(GL_TEXTURE1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depth_map_tex);
	for (unsigned int i = 0; i < 10; i++)
		cubes[i].draw();
	
	wood_diff_tex.use(GL_TEXTURE0);
	wood_diff_tex.use(GL_TEXTURE1); // Use as specular as well
	plane.draw();
}

unsigned int debug_quad_vao = 0;
unsigned int debug_quad_vbo;

void
debug_shadow_map_pass()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	depth_debug_shader.use();
	depth_debug_shader.set_uniform(
		"light_space_matrix", light_view_projection);
	if (debug_quad_vao == 0)
	{
		// Full screen quad
		float quadVertices[] = {
			// Positions        // Texture coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		};
		glGenVertexArrays(1, &debug_quad_vao);
		glGenBuffers(1, &debug_quad_vbo);
		glBindVertexArray(debug_quad_vao);
		glBindBuffer(GL_ARRAY_BUFFER, debug_quad_vbo);
		glBufferData(
			GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
			GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
			(void *)(3 * sizeof(float)));
	}
	glBindVertexArray(debug_quad_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depth_map_tex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
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
	if (window == nullptr)
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
	
	// Compile and link triangle shader
	try
	{
		material_shader.try_create_and_link();
		depth_map_shader.try_create_and_link();
		depth_debug_shader.try_create_and_link();
		white_shader.try_create_and_link();
		
		cont_diff_img.try_load();
		cont_spec_img.try_load();
		wood_diff_img.try_load();
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
	
	cont_diff_tex.load();
	cont_spec_tex.load();
	wood_diff_tex.load();
	
	cube_mesh.load();
	plane_mesh.load();
	
	// Framebuffer object for rendering a depth map
	// TODO: Extract depth map to Texture object
	glGenFramebuffers(1, &depth_map_fbo);
	glGenTextures(1, &depth_map_tex);
	glBindTexture(GL_TEXTURE_2D, depth_map_tex);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadow_map_width,
		shadow_map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		depth_map_tex, 0);
	// Disable draw/read for color data, we want only depth
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer generation failure." << std::endl;
		glfwTerminate();
		return -1;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// Prepare cubes
	for (int i = 0; i < 10; i++)
	{
		Model cube(&cube_mesh, &material_shader, cube_positions[i]);
		cubes.push_back(cube);
	}

	glEnable(GL_DEPTH_TEST);
	
	// Perform rendering loop
	while (!glfwWindowShouldClose(window))
	{
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
		
		handle_keyboard_input(window);
		
		// Update cube state
		for (unsigned int i = 0; i < 10; i++)
		{
			float angle = 20.0f * i;
			float rotation =
				(float)glfwGetTime() * glm::radians(50.0f)
				+ glm::radians(angle);
			cubes[i].rotation_axis = glm::vec3(0.5f, 1.0f, 0.0f);
			cubes[i].rotation_rad_angle = rotation;
		}
		
		shadow_map_pass();
		draw_objects_pass();
//		debug_shadow_map_pass();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteFramebuffers(1, &depth_map_fbo);
	glfwTerminate();
	
	return 0;
}
