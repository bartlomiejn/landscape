#ifndef LANDSCAPE_CUBE_H
#define LANDSCAPE_CUBE_H

#include <graphics/mesh.h>

class Cube : public Mesh
{
public:
	Cube();
	void load() override;
};

#endif //LANDSCAPE_CUBE_H
