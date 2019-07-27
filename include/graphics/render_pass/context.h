#ifndef LANDSCAPE_CONTEXT_H
#define LANDSCAPE_CONTEXT_H

#include <glm/glm.hpp>
#include <graphics/light.h>
#include <graphics/texture.h>

struct DrawObjectsContext
{
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 light_vp;
	glm::vec3 view_pos;
	DirectionalLight &light;
	unsigned int diff_slot;
	unsigned int spec_slot;
	unsigned int depthmap_slot;
	const Texture &depthmap_tex;
	
	DrawObjectsContext(
		glm::mat4 view,
		glm::mat4 projection,
		glm::mat4 light_vp,
		glm::vec3 view_pos,
		DirectionalLight &light,
		unsigned int diff_slot,
		unsigned int spec_slot,
		unsigned int depthmap_slot,
		const Texture &depthmap_tex
	) :
		view(view),
		projection(projection),
		light_vp(light_vp),
		view_pos(view_pos),
		light(light),
		diff_slot(diff_slot),
		spec_slot(spec_slot),
		depthmap_slot(depthmap_slot),
		depthmap_tex(depthmap_tex)
	{}
};

#endif //LANDSCAPE_CONTEXT_H
