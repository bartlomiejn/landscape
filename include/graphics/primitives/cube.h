#pragma once

#include <graphics/mesh.h>

namespace GFX 
{
/// A cube mesh.
class CubeMesh : public GFX::Mesh
{
public:
	/// Create a cube mesh instance.
	/// \param x_sz X size of the cube.
	/// \param y_sz Y size of the cube.
	/// \param z_sz Z size of the cube.
	/// \param uv_scale UV texture coordinate scale.
	CubeMesh(
		float x_sz = 0.5f, float y_sz = 0.5f, float z_sz = 0.5f,
		float uv_scale = 1.0f);
	
	virtual ~CubeMesh();

	/// Loads the cube mesh to the GPU.
	void load() override;
};
};