#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <range.h>
#include <graphics/shader.h>
#include <graphics/image.h>
#include <graphics/camera.h>
#include <graphics/texture.h>
#include <graphics/framebuffer.h>
#include <graphics/light.h>
#include <graphics/mesh.h>
#include <graphics/model.h>
#include <graphics/material.h>
#include <graphics/noise.h>
#include <graphics/primitives/cube.h>
#include <graphics/primitives/plane.h>
#include <graphics/render_pass/depth_map.h>
#include <graphics/render_pass/draw_objects.h>

// TODO LIST
//
// TODO: (PERFORMANCE) Instanced rendering of blocks
// TODO: (FEATURE) GUI
// TODO: (IMPROVE) Add texture slots to `Material`
// TODO: (IMPROVE) Extract mouse and keyboard input
// TODO: (IMPROVE) Extract depth map debug pass
// TODO: (IMPROVE) Extract graphics rendering
// TODO: (IMPROVE) Convert C pointers to shared pointers
// TODO: (IMPROVE) Find a way to use templates and still have header/implementation separation.
// TODO: (IMPROVE) Create platform-agnostic abstract types
// TODO: (BUG) Viewport should be scaled by 2x on Retina-like displays

const unsigned int window_width = 1280;
const unsigned int window_height = 720;
const unsigned int shadow_map_width = 2048;
const unsigned int shadow_map_height = 2048;

float delta_time = 0.0f; /// Time between current frame and last frame
float last_frame = 0.0f; /// Time of last frame

float mouse_last_x = window_width / 2;
float mouse_last_y = window_height / 2;
float mouse_sensitivity = 0.05f;

float movement_speed = 5.0f;

Camera camera(
	glm::vec3(-14.0f, 6.0f, 0.0f), 	// Position
	glm::vec3(0.0f, 1.0f, 0.0f), 	// World up vector
	0.0f, 				// Yaw
	0.0f, 				// Pitch
	45.0f); 			// FOV

glm::vec3 sunlight_dir = glm::vec3(8.0f, -20.0f, 4.0f);

DirectionalLight sunlight(
	camera.position(), 		// Look at point
	sunlight_dir,	     		// Reversed direction vector
	glm::vec3(0.2f, 0.2f, 0.2f), 	// Ambient
	glm::vec3(0.5f, 0.5f, 0.5f), 	// Diffuse
	glm::vec3(1.0f, 1.0f, 1.0f));	// Specular)

// Materials

Image grass("assets/grass-512.jpg");
Image ground_d("assets/blocks/ground_diff.jpg");
Texture ground_d_tex(&ground_d, layout_rgb, filter_linear);
Material ground_mtl(&ground_d_tex, &ground_d_tex, 128.0f);

// Shaders

MaterialShader mtl_shader("glsl/vertex.glsl");
Shader depth_map_shader(
	"glsl/shadowmap_vertex.glsl", "glsl/shadowmap_frag.glsl");
Shader depth_debug_shader(
	"glsl/shadowmap_vertex_quad.glsl", "glsl/shadowmap_frag_quad.glsl");
Shader white_shader("glsl/vertex.glsl", "glsl/white.glsl");

// Depth map

Texture depthmap_tex(
	nullptr, shadow_map_width, shadow_map_height, layout_depth16,
	filter_nearest);
Framebuffer depth_map_fb(depthmap_tex);
DepthMapRenderPass depth_map_pass(
	sunlight, depth_map_shader, depth_map_fb, shadow_map_width,
	shadow_map_height);

// Draw objects

std::vector<std::shared_ptr<Model>> models;
DrawObjectsRenderPass draw_pass(
	sunlight,
	mtl_shader,
	camera,
	depth_map_pass,
	window_width,
	window_height);

// Cubes

CubeMesh cube_mesh(1.0f, 1.0f, 1.0f);
std::vector<std::shared_ptr<Model>> cubes;

// Terrain

const int chunk_sz = 24;
float height_offset = -114.5f;
Noise::Perlin perlin;

Noise::Image  heightmap(perlin, chunk_sz, chunk_sz, layout_rgb, 4.0f, 6, 0.45f);
Texture       heightmap_tex(&heightmap, layout_rgb, filter_nearest);
Material      heightmap_mtl(&heightmap_tex, &heightmap_tex, 0.0f);

Noise::Volume<chunk_sz, chunk_sz, chunk_sz> chunk(
	perlin,	// Noise generator
	4.0f, 	// Initial frequency
	6, 	// Octave count
	0.4f, 	// Persistence
	0.6f);	// Threshold value for noise

std::vector<std::shared_ptr<Model>> terrain_blocks;

// Heightmap plane

PlaneMesh    plane_mesh((float)chunk_sz, (float)chunk_sz, 1.0f);
Model        plane(
	&plane_mesh, &mtl_shader, &heightmap_mtl, glm::vec3(-0.5f, -3.0f, -0.5f));

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
	glBindTexture(GL_TEXTURE_2D, depthmap_tex.id());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glBindVertexArray(0);
	
	glEnable(GL_DEPTH_TEST);
}

void update_sunlight()
{
	sunlight.look_at = camera.position();
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
		window_width, window_height, "Landscape", nullptr, nullptr);
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
		mtl_shader.try_create_and_link();
		depth_map_shader.try_create_and_link();
		depth_debug_shader.try_create_and_link();
		white_shader.try_create_and_link();
		
		grass.try_load();
		ground_d.try_load();
		
		ground_d_tex.load();
		depthmap_tex.load();
		heightmap_tex.load();
		
		cube_mesh.load();
		plane_mesh.load();
		
		depth_map_fb.try_load();
		depth_map_fb.set_draw(false);
		depth_map_fb.set_read(false);
	}
	catch (ShaderCompileFailure &err)
	{
		std::cout << "Shader compilation failure." << std::endl;
		glfwTerminate();
		return -1;
	}
	catch (ShaderLinkFailure &err)
	{
		std::cout << "Shader link failure." << std::endl;
		glfwTerminate();
		return -1;
	}
	catch (ImageLoadFailure &err)
	{
		std::cout << "Image loading failure: " << err.filename
			  << std::endl;
		glfwTerminate();
		return -1;
	}
	catch (FramebufferGenerationFailure &err)
	{
		std::cout << "Framebuffer generation failure." << std::endl;
		glfwTerminate();
		return -1;
	}
	
	// Prepare terrain blocks
	for (int ix = 0; ix < chunk_sz; ix++)
	for (int iy = 0; iy < chunk_sz; iy++)
	for (int iz = 0; iz < chunk_sz; iz++)
	{
		if (chunk.try_sample(ix, iy, iz) != 0)
		{
			glm::vec3 translation(
				(float)ix - (float)chunk_sz / 2.0f,
				(float)iy - (float)chunk_sz / 2.0f,
				(float)iz - (float)chunk_sz / 2.0f);
			
			auto cube_ptr = std::make_shared<Model>(
				&cube_mesh, &mtl_shader, &ground_mtl,
				translation);
			
			terrain_blocks.push_back(cube_ptr);
			models.push_back(cube_ptr);
		}
	}
	
	// Perform rendering loop
	while (!glfwWindowShouldClose(window))
	{
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
		
		handle_keyboard_input(window);
		
		update_sunlight();
		
		depth_map_pass.draw(models);
		draw_pass.draw(models);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	
	return 0;
}
