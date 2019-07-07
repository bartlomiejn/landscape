#ifndef LANDSCAPE_PLANE_H
#define LANDSCAPE_PLANE_H

#include <graphics/mesh.h>

class Plane : public Mesh
{
public:
	Plane();
	void load() override;
};

#endif //LANDSCAPE_PLANE_H
