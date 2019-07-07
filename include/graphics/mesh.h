#ifndef LANDSCAPE_MESH_H
#define LANDSCAPE_MESH_H

class Mesh
{
public:
	Mesh(float *verts, int verts_count, int vert_stride);
	~Mesh();
	void load();
	void use();
	void draw();
	void add_vertex_attrib_array(int index, int size, void *offset);
private:
	unsigned int vao;
	unsigned int vbo;
	float *vertices;
	int vertices_count;
	int vertex_stride;
};

#endif //LANDSCAPE_MESH_H
