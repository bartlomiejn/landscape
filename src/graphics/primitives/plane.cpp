#include <algorithm>
#include <graphics/primitives/plane.h>

const int plane_verts = 6;
const int plane_stride = 8;

/// Creates an array of plane vertices on the heap. Dispose after use.
/// \param x_sz X size of the plane.
/// \param z_sz Y size of the plane.
/// \param uv_scale UV texture coordinate scale.
/// \return Returns the pointer to the vertices array.
float *plane_vertices(float x_sz, float z_sz, float uv_scale)
{
	float verts[] = {
		// Positions          // Normals        // Tex coords
		x_sz,  0.0f,  z_sz, 0.0f, 1.0f, 0.0f, uv_scale, 0.0f,
		-x_sz, 0.0f,  z_sz, 0.0f, 1.0f, 0.0f, 0.0f,     0.0f,
		-x_sz, 0.0f, -z_sz, 0.0f, 1.0f, 0.0f, 0.0f,     uv_scale,
		
		x_sz,  0.0f,  z_sz, 0.0f, 1.0f, 0.0f, uv_scale, 0.0f,
		-x_sz, 0.0f, -z_sz, 0.0f, 1.0f, 0.0f, 0.0f,     uv_scale,
		x_sz,  0.0f, -z_sz, 0.0f, 1.0f, 0.0f, uv_scale, uv_scale
	};
	
	// Don't know how to create a heap array with the use of literals, so
	// I'll just copy a stack-allocated literal.
	float *heap_verts = new float[plane_verts * plane_stride];
	std::copy_n(verts, plane_verts * plane_stride, heap_verts);
	return heap_verts;
}

PlaneMesh::PlaneMesh(float x_sz, float y_sz, float uv_scale):
	Mesh(nullptr, plane_verts, plane_stride)
{
	vertices = plane_vertices(x_sz, y_sz, uv_scale);
}

PlaneMesh::~PlaneMesh()
{
	delete[] vertices;
}

void
PlaneMesh::load()
{
	Mesh::load();
	add_vertex_attrib_array(0, 3, (void*)nullptr);
	add_vertex_attrib_array(1, 3, (void*)(3 * sizeof(float)));
	add_vertex_attrib_array(2, 2, (void*)(6 * sizeof(float)));
}