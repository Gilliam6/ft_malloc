#include "../includes/internal.h"

size_t	print_allocations(void *bucket)
{
	size_t	bytes;
	t_chunk	*chunk;
	void	*ptr;

	bytes = 0;
	chunk = (t_chunk*)bucket;
	while (chunk)
	{
		if (chunk->meta & ALLOCED)
		{
			bytes += chunk->size;
			ptr = GET_MEM_POINTER(chunk);
			nomalloc_log(STDOUT, "0x%P - 0x%P : %zu bytes\n"
						, ptr
						, ((void*)((char*)ptr + chunk->size))
						, chunk->size);
		}
		chunk = chunk->next;
	}
	return (bytes);
}

void	show_alloc_mem(void)
{
	size_t	total;

	nomalloc_log(STDOUT, "TINY: 0x%P\n", g_buckets.tiny_bucket);
	total = print_allocations(g_buckets.tiny_bucket);
	nomalloc_log(STDOUT, "SMALL: 0x%P\n", g_buckets.small_bucket);
	total += print_allocations(g_buckets.small_bucket);
	nomalloc_log(STDOUT, "LARGE: 0x%P\n", g_buckets.large_bucket);
	total += print_allocations(g_buckets.large_bucket);
	nomalloc_log(STDOUT, "Total : %zu bytes\n", total);
}
