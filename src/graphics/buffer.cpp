#include <graphics/buffer.h>

GPU::Buffer::Buffer() : buffer_size(0)
{
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
}

GPU::Buffer::Buffer(void *items, size_t item_size, unsigned long items_count) :
	buffer_size(items_count * item_size)
{
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, items, GL_DYNAMIC_DRAW);
}

void
GPU::Buffer::use()
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void
GPU::Buffer::update(
	void *items, size_t item_size, unsigned long items_count
){
	use();
	if (item_size * items_count > buffer_size)
	{
		buffer_size = item_size * items_count;
		glBufferData(
			GL_ARRAY_BUFFER, buffer_size, items, GL_DYNAMIC_DRAW);
	}
	else
	{
		glBufferSubData(
			GL_ARRAY_BUFFER, 0, item_size * items_count, items);
	}
};