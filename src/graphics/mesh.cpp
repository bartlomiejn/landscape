#include <glad/glad.h>
#include <glm/glm.hpp>
#include <graphics/mesh.h>

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
Mesh::add_vertex_attrib_array(int index, int size, void *offset)
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(
		index, size, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_stride,
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
	glDrawArrays(GL_TRIANGLES, 0, vertices_count);
}

void Mesh::draw(
	GPU::Buffer &model_matrix_buffer, unsigned long instance_count
) const {
	// This is shader dependent and shouldn't be hardcoded here, but should
	// be sufficient for now.
	unsigned int model_mat_idx = 3;
	
	// Bind the VAO and the buffer.
	use();
	model_matrix_buffer.use();
	
	// To pass a mat4 to the shader as a attribute, we actually have to set
	// four vectors taking 4 layout slots.
	for (unsigned int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(model_mat_idx + 0);
		glVertexAttribPointer(
			model_mat_idx + 0, 4, GL_FLOAT, GL_FALSE,
			sizeof(glm::mat4),
			(const GLvoid*)(sizeof(GLfloat) * 4 * i));
		// Make the attribute advance once per instance, rather than
		// once per vertex (which the default value of 0 does).
		glVertexAttribDivisor(model_mat_idx + i, 1);
	}
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertices_count, instance_count);
}
