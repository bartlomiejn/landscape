#include <graphics/mesh.h>
#include <glad/glad.h>

Mesh::Mesh(float *verts, int verts_count, int vert_stride) :
	vertices(verts),
	vertices_count(verts_count),
	vertex_stride(vert_stride)
{}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
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
		sizeof(float) * vertices_count * vertex_stride,
		vertices,
		GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void
Mesh::use()
{
	glBindVertexArray(vao);
}

void Mesh::draw()
{
	glDrawArrays(GL_TRIANGLES, 0, vertices_count);
}

void
Mesh::add_vertex_attrib_array(int index, int size, void *offset)
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(
		index, size, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_stride,
		offset);
	glBindVertexArray(0);
}