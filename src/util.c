#include "../includes/internal.h"

size_t			ft_intmaxtoa_base(char *string
							, intmax_t nbr
							, int8_t base
							, const char *lookup)
{
	uintmax_t	index;
	size_t		len;
	size_t		out;

	index = nbr;
	len = 1;
	while ((index /= base))
		len++;
	out = len;
	index = nbr;
	string[len] = '\0';
	string[--len] = lookup[index % base];
	while ((index /= base))
		string[--len] = lookup[index % base];
	return (out);
}

// static size_t	copy_by_chunk(char **dst, const char **src, size_t length)
// {
// 	long		*aligned_dst;
// 	const long	*aligned_src;

// 	aligned_dst = (long*)*dst;
// 	aligned_src = (long*)*src;
// 	while (length >= BIGBLOCKSIZE)
// 	{
// 		*aligned_dst++ = *aligned_src++;
// 		*aligned_dst++ = *aligned_src++;
// 		*aligned_dst++ = *aligned_src++;
// 		*aligned_dst++ = *aligned_src++;
// 		length -= BIGBLOCKSIZE;
// 	}
// 	*dst = (char*)aligned_dst;
// 	*src = (char*)aligned_src;
// 	return (length);
// }
int				contains_chunk(t_chunk *bucket, t_chunk *chunk)
{
	while (bucket)
	{
		if (chunk == bucket)
			return (1);
		bucket = bucket->next;
	}
	return (0);
}

int				is_chunk(t_chunk *chunk)
{
	t_chunk	*bucket;

	if ((bucket = (t_chunk*)g_buckets.large_bucket) && contains_chunk(bucket, chunk))
		return (1);
	else if ((bucket = (t_chunk*)g_buckets.small_bucket) && contains_chunk(bucket, chunk))
		return (1);
	else if ((bucket = (t_chunk*)g_buckets.tiny_bucket) && contains_chunk(bucket, chunk))
		return (1);
	return (0);
}
