#include <algorithm>
#include <graphics/primitives/cube.h>

using namespace GFX;

const int cube_verts = 36;
const int cube_stride = 8;

/// Creates an array of cube vertices on the heap. Dispose after use.
/// \param x_sz X size of the cube.
/// \param z_sz Y size of the cube.
/// \param uv_scale UV texture coordinate scale.
/// \return Returns the pointer to the vertices array.
float *cube_vertices(float x_sz, float y_sz, float z_sz, float uv_scale)
{
	x_sz /= 2;
	y_sz /= 2;
	z_sz /= 2;
	float verts[] = {
		// Positions          // Normals           // Tex coords
		-x_sz, -y_sz, -z_sz,  0.0f,  0.0f,  -1.0f, 0.0f,     0.0f, // back
		x_sz,  -y_sz, -z_sz,  0.0f,  0.0f,  -1.0f, uv_scale, 0.0f,
		x_sz,  y_sz,  -z_sz,  0.0f,  0.0f,  -1.0f, uv_scale, uv_scale,
		x_sz,  y_sz,  -z_sz,  0.0f,  0.0f,  -1.0f, uv_scale, uv_scale,
		-x_sz, y_sz,  -z_sz,  0.0f,  0.0f,  -1.0f, 0.0f,     uv_scale,
		-x_sz, -y_sz, -z_sz,  0.0f,  0.0f,  -1.0f, 0.0f,     0.0f,
		
		-x_sz, -y_sz, z_sz,   0.0f,  0.0f,  1.0f,  0.0f,     0.0f, // front
		x_sz,  -y_sz, z_sz,   0.0f,  0.0f,  1.0f,  uv_scale, 0.0f,
		x_sz,  y_sz,  z_sz,   0.0f,  0.0f,  1.0f,  uv_scale, uv_scale,
		x_sz,  y_sz,  z_sz,   0.0f,  0.0f,  1.0f,  uv_scale, uv_scale,
		-x_sz, y_sz,  z_sz,   0.0f,  0.0f,  1.0f,  0.0f,     uv_scale,
		-x_sz, -y_sz, z_sz,   0.0f,  0.0f,  1.0f,  0.0f,     0.0f,
		
		-x_sz,  y_sz, z_sz,   -1.0f, 0.0f,  0.0f,  uv_scale, 0.0f, // left
		-x_sz,  y_sz, -z_sz,  -1.0f, 0.0f,  0.0f,  uv_scale, uv_scale,
		-x_sz, -y_sz, -z_sz,  -1.0f, 0.0f,  0.0f,  0.0f,     uv_scale,
		-x_sz, -y_sz, -z_sz,  -1.0f, 0.0f,  0.0f,  0.0f,     uv_scale,
		-x_sz, -y_sz, z_sz,   -1.0f, 0.0f,  0.0f,  0.0f,     0.0f,
		-x_sz,  y_sz, z_sz,   -1.0f, 0.0f,  0.0f,  uv_scale, 0.0f,
		
		x_sz,  y_sz,  z_sz,   1.0f,  0.0f,  0.0f,  uv_scale, 0.0f, // right
		x_sz,  y_sz,  -z_sz,  1.0f,  0.0f,  0.0f,  uv_scale, uv_scale,
		x_sz,  -y_sz, -z_sz,  1.0f,  0.0f,  0.0f,  0.0f,     uv_scale,
		x_sz,  -y_sz, -z_sz,  1.0f,  0.0f,  0.0f,  0.0f,     uv_scale,
		x_sz,  -y_sz, z_sz,   1.0f,  0.0f,  0.0f,  0.0f,     0.0f,
		x_sz,  y_sz,  z_sz,   1.0f,  0.0f,  0.0f,  uv_scale, 0.0f,
		
		-x_sz, -y_sz, -z_sz,  0.0f,  -1.0f, 0.0f,  0.0f,     uv_scale, // down
		x_sz, -y_sz,  -z_sz,  0.0f,  -1.0f, 0.0f,  uv_scale, uv_scale,
		x_sz, -y_sz,  z_sz,   0.0f,  -1.0f, 0.0f,  uv_scale, 0.0f,
		x_sz, -y_sz,  z_sz,   0.0f,  -1.0f, 0.0f,  uv_scale, 0.0f,
		-x_sz, -y_sz, z_sz,   0.0f,  -1.0f, 0.0f,  0.0f,     0.0f,
		-x_sz, -y_sz, -z_sz,  0.0f,  -1.0f, 0.0f,  0.0f,     uv_scale,
		
		-x_sz, y_sz, -z_sz,   0.0f,  1.0f,  0.0f,  0.0f,     uv_scale, // up
		x_sz,  y_sz, -z_sz,   0.0f,  1.0f,  0.0f,  uv_scale, uv_scale,
		x_sz,  y_sz, z_sz,    0.0f,  1.0f,  0.0f,  uv_scale, 0.0f,
		x_sz,  y_sz, z_sz,    0.0f,  1.0f,  0.0f,  uv_scale, 0.0f,
		-x_sz, y_sz, z_sz,    0.0f,  1.0f,  0.0f,  0.0f,     0.0f,
		-x_sz, y_sz, -z_sz,   0.0f,  1.0f,  0.0f,  0.0f,     uv_scale
	};
	
	float *heap_verts = new float[cube_verts * cube_stride];
	std::copy_n(verts, cube_verts * cube_stride, heap_verts);
	return heap_verts;
}

CubeMesh::CubeMesh(float x_sz, float y_sz, float z_sz, float uv_scale) :
	Mesh(cube_vertices(x_sz, y_sz, z_sz, uv_scale), cube_verts, cube_stride)
{}

CubeMesh::~CubeMesh()
{
	delete this->verts;
}

void
CubeMesh::load()
{
	Mesh::load();
	add_vertex_attrib_array(0, 3, (void *)nullptr);
	add_vertex_attrib_array(1, 3, (void *)(3 * sizeof(float)));
	add_vertex_attrib_array(2, 2, (void *)(6 * sizeof(float)));
}