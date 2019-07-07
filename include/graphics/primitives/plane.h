#ifndef LANDSCAPE_PLANE_H
#define LANDSCAPE_PLANE_H

#include <graphics/mesh.h>

class PlaneMesh : public Mesh
{
public:
	PlaneMesh();
	void load() override;
};

#endif //LANDSCAPE_PLANE_H
