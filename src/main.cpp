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
#include <graphics/framebuffer.h>
#include <graphics/light.h>
#include <graphics/mesh.h>
#include <graphics/model.h>
#include <graphics/perlin_noise.h>
#include <graphics/primitives/cube.h>
#include <graphics/primitives/plane.h>
#include <graphics/render_pass/depth_map.h>

// TODO LIST
//
// TODO: (FEATURE) Procedural terrain generation
// TODO: (IMPROVE) Extract mouse and keyboard input
// TODO: (IMPROVE) Extract other render passes
// TODO: (IMPROVE) Extract graphics rendering
// TODO: (IMPROVE) Create platform-agnostic abstract types
// TODO: (IMPROVE) Convert C pointers to shared pointers
// TODO: (IMPROVE) Find a way to use templates and still have header/implementation separation.
// TODO: (BUG) Shading is based on a point light, but shadow map is directional
// TODO: (BUG) Viewport should be scaled by 2x on Retina-like displays

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

Texture cont_diff_tex(&cont_diff_img, layout_rgba, filter_linear);
Texture cont_spec_tex(&cont_spec_img, layout_rgba, filter_linear);
Texture wood_diff_tex(&wood_diff_img, layout_rgba, filter_linear);

CubeMesh cube_mesh;
PlaneMesh plane_mesh;

std::vector<std::shared_ptr<Model>> models;
std::vector<std::shared_ptr<Model>> cubes;
Model plane(&plane_mesh, &material_shader, glm::vec3(0.0f, -2.4f, 0.0f));

PerlinNoiseGenerator noise_generator;

// Depth map

Texture depth_map_tex(
	nullptr, shadow_map_width, shadow_map_height, layout_depth16,
	filter_nearest);
Framebuffer depth_map_fb(&depth_map_tex);
DepthMapRenderPass depth_map_pass(
	shadow_map_spot_light, depth_map_shader, depth_map_fb, shadow_map_width,
	shadow_map_height);

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
draw_objects_pass()
{
	glViewport(0, 0, window_width, window_height);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);
	
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
	material_shader.set_uniform(
		"light_space_matrix", depth_map_pass.light_view_projection());
	material_shader.set_uniform("view_pos", camera.position());
	material_shader.set_spot_light(shadow_map_spot_light);
	material_shader.set_uniform("material.diffuse", 0);
	material_shader.set_uniform("material.specular", 1);
	material_shader.set_uniform("material.shininess", 32.0f);
	material_shader.set_uniform("shadow_map", 2);
	
	cont_diff_tex.use(GL_TEXTURE0);
	cont_spec_tex.use(GL_TEXTURE1);
	depth_map_tex.use(GL_TEXTURE2);
	for (unsigned int i = 0; i < 10; i++)
		cubes[i].get()->draw();
	
	wood_diff_tex.use(GL_TEXTURE0);
	wood_diff_tex.use(GL_TEXTURE1); // Use as specular as well
	plane.draw();
}

unsigned int debug_quad_vao = 0;
unsigned int debug_quad_vbo;

void
debug_shadow_map_pass()
{
	glDisable(GL_DEPTH_TEST);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	depth_debug_shader.use();
	depth_debug_shader.set_uniform(
		"light_space_matrix", depth_map_pass.light_view_projection());
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
	glBindTexture(GL_TEXTURE_2D, depth_map_tex.id());
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
	
	// Load all image assets, create textures and framebuffers
	try
	{
		material_shader.try_create_and_link();
		depth_map_shader.try_create_and_link();
		depth_debug_shader.try_create_and_link();
		white_shader.try_create_and_link();
		
		cont_diff_img.try_load();
		cont_spec_img.try_load();
		wood_diff_img.try_load();
		
		depth_map_tex.load();
		cont_diff_tex.load();
		cont_spec_tex.load();
		wood_diff_tex.load();
		
		cube_mesh.load();
		plane_mesh.load();
		
		depth_map_fb.try_load();
		depth_map_fb.set_draw(false);
		depth_map_fb.set_read(false);
	}
	catch (ShaderCompileFailure err)
	{
		std::cout << "Shader compilation failure." << std::endl;
		glfwTerminate();
		return -1;
	}
	catch (ShaderLinkFailure err)
	{
		std::cout << "Shader link failure." << std::endl;
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
	catch (FramebufferGenerationFailure err)
	{
		std::cout << "Framebuffer generation failure." << std::endl;
		glfwTerminate();
		return -1;
	}
	
	// Prepare drawables vector
	for (int i = 0; i < 10; i++)
	{
		Model cube(&cube_mesh, &material_shader, cube_positions[i]);
		std::shared_ptr<Model> cube_ptr = std::make_shared<Model>(cube);
		cubes.push_back(cube_ptr);
		models.push_back(cube_ptr);
	}
	models.push_back(std::make_shared<Model>(plane));
	
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
			cubes[i].get()->rotation_axis =
				glm::vec3(0.5f, 1.0f, 0.0f);
			cubes[i].get()->rotation_rad_angle = rotation;
		}
		
		depth_map_pass.draw(models);
		draw_objects_pass();
//		debug_shadow_map_pass();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	
	return 0;
}
