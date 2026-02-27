#include "../includes/internal.h"

void	*realloc_chunk(void *pointer, size_t size);

void	*realloc(void *pointer, size_t size)
{
	if ((size_t)pointer % 8)
	{
		DEBUG_LOG("Realloc attempted on misaligned pointer: %p\n", pointer);
		return (NULL);
	}
	else if (pointer == NULL)
	{
		DEBUG_LOG("Realloc returning new pointer of size: %zu\n", size);
		return (malloc(size));
	}
	return (realloc_chunk(pointer, size));
}

void	*realloc_chunk(void *pointer, size_t size)
{
	t_chunk	*old_chunk;
	void	*new;
	size_t	new_size;

	old_chunk = GET_CHUNK_POINTER(pointer);
	new_size = MIN(size, ALIGNED_SIZE(old_chunk->size));
	if ((new = malloc(size)) != NULL)
	{
		ft_memcpy((void*)new, pointer, new_size);
		DEBUG_LOG("Realloc moved %zu bytes from %p to pointer %p of size: %zu\n"
			, new_size
			, pointer
			, new
			, size);
		free(pointer);
	}
	else
		DEBUG_LOG("Realloc failed to allocate pointer of size: %zu\n", size);
	return (new);
}