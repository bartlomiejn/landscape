#ifndef LANDSCAPE_PLANE_H
#define LANDSCAPE_PLANE_H

#include <graphics/mesh.h>

class PlaneMesh : public Mesh
{
public:
	/// Creates a plane mesh.
	/// \param x_sz X size value
	/// \param y_sz Y size value
	/// \param uv_scale Texture coordinates scale
	PlaneMesh(
		float x_sz = 25.0f, float y_sz = 25.0f, float uv_scale = 25.0f);
	
	/// Destroys a plane mesh.
	~PlaneMesh();
	
	/// Loads the mesh to the GPU.
	void load() override;
};

#endif //LANDSCAPE_PLANE_H
