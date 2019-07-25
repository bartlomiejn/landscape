#ifndef LANDSCAPE_MATERIAL_H
#define LANDSCAPE_MATERIAL_H

#include <graphics/texture.h>

class Material
{
public:
	Texture *diffuse;
	Texture *specular;
	float shininess;
	
	Material(Texture *diffuse, Texture *specular, float shininess) :
		diffuse(diffuse), specular(specular), shininess(shininess)
	{}
};

#endif //LANDSCAPE_MATERIAL_H
