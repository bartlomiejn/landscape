#ifndef LANDSCAPE_MODEL_GROUP_H
#define LANDSCAPE_MODEL_GROUP_H

#include <vector>
#include <graphics/model.h>
#include <graphics/buffer.h>

/// Groups several models which use the same mesh, shader and materials,
/// allowing for instanced drawing which increases performance. Not thread safe.
class ModelGroup : public Drawable
{
public:
	/// Create an empty ModelGroup.
	ModelGroup();
	
	/// Create a ModelGroup from a container of Model pointers.
	ModelGroup(std::vector<std::shared_ptr<Model>> models);
	
	/// Add a Model to the ModelGroup.
	void push_back(std::shared_ptr<Model> model);
	
	/// Removes a Model from the ModelGroup.
	void remove(std::shared_ptr<Model> model);
	
	// Drawable
	
	/// Draw in a draw objects context.
	void draw(DrawObjectsContext &ctx) final;
	
	/// Draw overriding the stored shader. Used in depth map pass.
	void draw(Shader &override_shader) final;
private:
	std::vector<std::shared_ptr<Model>> models;
	GPU::Buffer buffer;
	
	void update_buffer();
};

#endif //LANDSCAPE_MODEL_GROUP_H
