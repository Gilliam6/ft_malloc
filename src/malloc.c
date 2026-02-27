#include "../includes/internal.h"

t_buckets		g_buckets;

void    *proceed_ptr(size_t size, void **bucket, size_t bucket_size);
void    *proceed_large_bucket(size_t size);
void	split_block(t_chunk *chunk, size_t size);

void	*malloc(size_t size)
{
	void	**bucket;
	size_t	bucket_size;

	DEBUG_LOG("Malloc call for %zu bytes\n", size);
    //check size and define bucket 
	if (size <= 0)
		return (NULL);
	else if (LARGE_BUCKET(size))
		return (proceed_large_bucket(size));
	bucket = TINY_BUCKET(size) ? &g_buckets.tiny_bucket : &g_buckets.small_bucket;
	bucket_size = TINY_BUCKET(size) ? TINY_SIZE : SMALL_SIZE;
	return (proceed_ptr(size, bucket, bucket_size));
}

void	*proceed_ptr(size_t size, void **bucket, size_t bucket_size)
{
	void	*ptr;
	t_chunk	*chunk;
	t_chunk	*last;
	size_t	aligned;

	DEBUG_PRINT("Proceeding memory for bucket allocation\n");
    //check if memory size aligned and get the aligned size
	aligned = ALIGNED_SIZE(size);
	last = (t_chunk*)*bucket;
    //check if bucket is empty and request space
	if (!*bucket && !(*bucket = request_space(NULL, bucket_size)))
		return (NULL);
	//check if there is a free chunk with free aligned spacein the bucket
	if ((chunk = next_free_chunk(&last, aligned, *bucket)) == NULL)
	{
		if ((chunk = request_space(last, PAGE_SIZE)) == NULL)
			return (chunk);
	}
    // split chunk on blocks if there is extra space
	if (ALIGNED_SIZE(chunk->size) > META_SIZE + aligned + OFFSET)
		split_block(chunk, aligned);
	chunk->size = size;
	chunk->meta |= (ALLOCED | (TINY_BUCKET(size) ? TINY_BUCKET : SMALL_BUCKET));
	ptr = GET_MEM_POINTER(chunk);
	if ((size_t)ptr % alignof(max_align_t))
		DEBUG_PRINT("ERROR: Unaligned ptr procured\n");
	DEBUG_LOG("Malloc returning pointer: %p\n", ptr);
	return (ptr);
}

void	*proceed_large_bucket(size_t size)
{
	t_chunk	*last;
	t_chunk	*chunk;
	void	*ptr;

	DEBUG_PRINT("Procuring memory for large allocation\n");
	if (!g_buckets.large_bucket && !(g_buckets.large_bucket = request_space(
		NULL, META_SIZE + OFFSET + size)))
		return (NULL);
	if ((chunk = next_free_chunk(&last, size, g_buckets.large_bucket)) == NULL)
	{
		if ((chunk = request_space(last, META_SIZE + OFFSET + size)) == NULL)
			return (chunk);
	}
	chunk->meta |= (ALLOCED | LARGE_BUCKET);
	chunk->size = size;
	ptr = GET_MEM_POINTER(chunk);
    
	DEBUG_LOG("Malloc returning pointer: %p\n", ptr);
	return (ptr);
}

void	split_block(t_chunk *chunk, size_t size)
{
	char	*next;
	t_chunk	*next_chunk;

	size += META_SIZE + OFFSET;
	next = (char*)(chunk) + size;
	next_chunk = (t_chunk*)next;
	next_chunk->size = ALIGNED_SIZE(chunk->size) - size;
	next_chunk->meta = 0;
	next_chunk->next = NULL;
	if (chunk->next != NULL)
		next_chunk->next = chunk->next;
	chunk->next = next_chunk;
	DEBUG_LOG("Chunk %p extra space broken into new chunk %p\n"
			, (void*)chunk
			, (void*)next_chunk);
}