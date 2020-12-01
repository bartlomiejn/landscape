#include <stdlib.h>
#include <graphics/mesh.h>
#include <glad/glad.h>

using namespace GFX;

Mesh::Mesh() : verts(nullptr), verts_count(0), vert_stride(0) {}

Mesh::Mesh(float *verts, int verts_count, int vert_stride) :
	verts(verts),
	verts_count(verts_count),
	vert_stride(vert_stride) {}

Mesh::~Mesh()
{
	if (vao != UINT_MAX && vbo != UINT_MAX)
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}
}

void Mesh::set(float *verts, int verts_count, int vert_stride)
{
	this->verts = verts;
	this->verts_count = verts_count;
	this->vert_stride = vert_stride;
}

void
Mesh::load()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(float) * verts_count * vert_stride,
		verts,
		GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void
Mesh::add_vertex_attrib_array(int index, int size, void *offset)
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(
		index, size, GL_FLOAT, GL_FALSE, sizeof(float) * vert_stride,
		offset);
	glBindVertexArray(0);
}

void
Mesh::use() const
{
	glBindVertexArray(vao);
}

void Mesh::draw() const
{
	glDrawArrays(GL_TRIANGLES, 0, verts_count);
}
