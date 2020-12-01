#pragma once

#include <limits.h>

namespace GFX 
{
class Mesh
{
public:
	/// Creates an empty mesh object.
	Mesh();

	/// Creates a mesh from provided vertices.
	/// \param verts Vertex buffer. Does not take ownership of the buffer.
	/// \param verts_count Vertices count
	/// \param vert_stride Stride of a single vertex
	Mesh(float *verts, int verts_count, int vert_stride);
	
	/// Destroys the mesh.
	virtual ~Mesh();

	/// Setup the vertex buffer.
	void set(float *verts, int verts_count, int vert_stride);
	
	/// Loads the mesh to the GPU.
	virtual void load();
	
	/// Adds a vertex attribute array to the GPU.
	void add_vertex_attrib_array(int index, int size, void *offset);
	
	/// Sets the mesh as active.
	void use() const;
	
	/// Draws using the mesh.
	void draw() const;
protected:
	float *verts;
	int verts_count;
	int vert_stride;
private:
	unsigned int vao = UINT_MAX; ///< Internal vertex array object identifier.
	unsigned int vbo = UINT_MAX; ///< Internal vertex buffer object identifier.
};
};