#ifndef LANDSCAPE_MESH_H
#define LANDSCAPE_MESH_H

#include <graphics/buffer.h>

class Mesh
{
public:
	/// Creates a mesh from provided vertex information.
	/// \param verts Vertex buffer. Does not take ownership of the buffer.
	/// Dispose manually.
	/// \param verts_count Vertices count
	/// \param vert_stride Stride of a single vertex
	Mesh(float *verts, int verts_count, int vert_stride);
	
	/// Destroys the mesh.
	~Mesh();
	
	/// Loads the mesh to the GPU.
	virtual void load();
	
	/// Adds a vertex attribute array to the GPU.
	void add_vertex_attrib_array(int index, int size, void *offset);
	
	/// Sets the mesh as active.
	void use() const;
	
	/// Draws using the mesh.
	void draw() const;
	
	/// Performs instanced drawing using the mesh.
	/// \param model_matrix_buffer Model matrix buffer to use.
	/// \param instance_count Instance count.
	void draw(
		GPU::Buffer &model_matrix_buffer, unsigned long instance_count
	) const;
	
protected:
	float *vertices;
	int vertices_count;
	int vertex_stride;
	
private:
	unsigned int vao; ///< Internal vertex array object identifier.
	unsigned int vbo; ///< Internal vertex buffer object identifier.
};

#endif //LANDSCAPE_MESH_H
