#ifndef LANDSCAPE_BUFFER_H
#define LANDSCAPE_BUFFER_H

#include <algorithm>
#include <glad/glad.h>

namespace GPU
{
	class Buffer
	{
	public:
		static const unsigned int min_size = 8;
		
		/// Creates an empty buffer on the GPU.
		Buffer();
		
		/// Creates a buffer on the GPU with initial content.
		/// \param items Initial items array to put into the buffer.
		/// \param item_size Size of a single item.
		/// \param items_count Item count.
		Buffer(
			void *items, size_t item_size,
			unsigned long items_count);
		
		/// Update the buffer contents.
		/// \param items
		/// \param item_size
		/// \param items_count
		void update(
			void *items, size_t item_size,
			unsigned long items_count);
	private:
		unsigned int id;
		unsigned long buffer_size;
	};
}

#endif //LANDSCAPE_BUFFER_H
