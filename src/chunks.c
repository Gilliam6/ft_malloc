#include "../includes/internal.h"

/*
** Wrapper to mmap to detect failed mapping
*/

void	*alloc_mem(size_t size)
{
	void	*ret;

	ret = mmap(NULL, size, MMAP_PROTECTIONS, MMAP_FLAGS, 0, 0);
	if (ret == MAP_FAILED)
		return (NULL);
	return (ret);
}

t_chunk	*request_space(t_chunk *last, size_t size)
{
	t_chunk	*chunk;

	// DEBUG_LOG("Calling mmap for %zu bytes\n", size);
	if ((chunk = (t_chunk*)alloc_mem(size)) == NULL)
		return (chunk);
	if (last)
		last->next = chunk;
	chunk->size = size;
	chunk->next = NULL;
	chunk->meta = 0;
	chunk->meta |= BLOCK;
	return (chunk);
}

t_chunk	*next_free_chunk(t_chunk **last, size_t size, void *bin)
{
	t_chunk	*curr;

	curr = (t_chunk *)bin;
	while (curr &&
		(!!(curr->meta & ALLOCED) || (ALIGNED_SIZE(curr->size) < size)))
	{
		*last = curr;
		curr = curr->next;
	}
	if (curr != NULL)
		DEBUG_LOG("Free chunk found to allocate: %p\n", (void*)curr);
	return (curr);
}

