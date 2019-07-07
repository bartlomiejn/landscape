#ifndef LANDSCAPE_CUBE_H
#define LANDSCAPE_CUBE_H

#include <graphics/mesh.h>

class CubeMesh : public Mesh
{
public:
	CubeMesh();
	void load() override;
};

#endif //LANDSCAPE_CUBE_H
