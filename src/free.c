#include "../includes/internal.h"

void	free_large_bucket(t_chunk *chunk);
void	merge_bucket(void **bucket);
void	find_free_buckets(void **bucket);
void	free_chunk(void **bin, t_chunk *c, t_chunk *n);

void	free(void *pointer)
{
	void	*bin;
	t_chunk	*chunk;

	if (pointer == NULL || (size_t)pointer % 8)
		return ;
	chunk = GET_CHUNK_POINTER(pointer); // get header pointer 
	DEBUG_LOG("Freeing pointer: %p from chunk: %p\n", pointer, (void*)chunk);
	if (!is_chunk(chunk) || !(chunk->meta & ALLOCED))
	{
		DEBUG_LOG("Error: Attempt to free unallocated chunk %p\n", pointer);
		return ;
	}
	chunk->meta &= ~ALLOCED; // set the allocated flag to 0
	if (!(chunk->meta & LARGE_BUCKET))
	{
		bin = chunk->meta & TINY_BUCKET ? &g_buckets.tiny_bucket : &g_buckets.small_bucket;
		merge_bucket(bin);
	}
	else
		free_large_bucket(chunk);
}

void	merge_bucket(void **bucket)
{
	t_chunk	*next;
	t_chunk	*current;

	current = *(t_chunk**)bucket;
	while (current)
	{
		next = current->next;
		while (current && !(current->meta & ALLOCED) && next && !(next->meta & ALLOCED))
		{
			DEBUG_LOG("Combining chunks %p, %p\n", (void*)current, (void*)next);
			current->size += META_SIZE + OFFSET + ALIGNED_SIZE(next->size);
			current->next = next->next;
			next = next->next;
		}
		current = next;
	}
	find_free_buckets(bucket);
}

void	find_free_buckets(void **bucket)
{
	t_chunk	*current;
	t_chunk	*next;
	int		in_use;

	current = *(t_chunk**)bucket;
	while (current)
	{
		next = current->next;
		if (!(current->meta & ALLOCED) && (!next || !(next->meta & BLOCK)))
		{
			in_use = 0;
			while (next && !(next->meta & BLOCK))
			{
				if (next->meta & ALLOCED)
				{
					in_use = 1;
					break ;
				}
				next = next->next;
			}
			if (!in_use && (current->meta & BLOCK))
				free_chunk(bucket, current, next);
		}
		current = next;
	}
}

void	free_chunk(void **bin, t_chunk *c, t_chunk *n)
{
	int		val;
	t_chunk	*p;

	if ((void*)c == *bin)
		return ;
	p = *(t_chunk**)bin;
	while (p && p->next != c)
		p = p->next;
	p->next = n;
	val = munmap(c, META_SIZE + OFFSET + ALIGNED_SIZE(c->size));
	DEBUG_LOG("Munmap block %p: %d\n", (void*)c, val);
	(void)val;
}

void	free_large_bucket(t_chunk *chunk)
{
	t_chunk	*current;
	int		val;

	current = g_buckets.large_bucket;
	while (current && current->next && current->next != chunk)
		current = current->next;
	if (g_buckets.large_bucket == chunk)
		g_buckets.large_bucket = chunk->next;
	else if (current)
		current->next = chunk->next;
	val = munmap(chunk, META_SIZE + OFFSET + ALIGNED_SIZE(chunk->size));
	if (val == -1)
		DEBUG_LOG("Error Munmapping large chunk %p\n", (void*)chunk);
	else
		DEBUG_LOG("Munmap large chunk %p: %d\n", (void*)chunk, val);
}