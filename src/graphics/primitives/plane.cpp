#include <graphics/primitives/plane.h>

float plane_verts[] = {
	// Positions           // Normals         // Tex coords
	25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	-25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
	-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
	
	25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
	25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
};

PlaneMesh::PlaneMesh() : Mesh(plane_verts, 6, 8) {}

void
PlaneMesh::load()
{
	Mesh::load();
	add_vertex_attrib_array(0, 3, (void*)nullptr);
	add_vertex_attrib_array(1, 3, (void*)(3 * sizeof(float)));
	add_vertex_attrib_array(2, 2, (void*)(6 * sizeof(float)));
}