#ifndef MALLOC_INTERNAL_H
# define MALLOC_INTERNAL_H

/*
** stdarg.h - va_list
** stddef.h - NULL, alignof, max_align_t
** unistd.h - mmap, getpagesize
** sys/mman.h - PROT_READ, PROT_WRITE, MAP_ANON, MAP_PRIVATE
** errno.h - errno
** stdalign.h - alignof, max_align_t
** stdint.h - intmax_t, uintmax_t
*/

# include <stdarg.h>
# include <stddef.h>
# include <unistd.h>
# include <sys/mman.h>
# include <errno.h>
# include <stdalign.h>
# include <stdint.h>
# include "../libft/libft.h"

typedef struct		s_buckets {
	void			*tiny_bucket;
	void			*small_bucket;
	void			*large_bucket;
}					t_buckets;

typedef struct		s_chunk {
	struct s_chunk	*next;
	size_t			size;
	unsigned		meta;
}					t_chunk;

extern t_buckets		g_buckets; // only declared here, defined in malloc.c

enum	e_block_flags {
	ALLOCED = (1u << 0),
	BLOCK = (1u << 1),
	TINY_BUCKET = (1u << 2),
	SMALL_BUCKET = (1u << 3),
	LARGE_BUCKET = (1u << 4),
};

size_t				ft_intmaxtoa_base(char *string
    , intmax_t nbr
    , int8_t base
    , const char *lookup);
t_chunk				*request_space(t_chunk *last, size_t size);
t_chunk				*next_free_chunk(t_chunk **last, size_t size, void *bin);
int					is_chunk(t_chunk *chunk);


void				nomalloc_log(int fd, const char *fmt, ...);
void				show_alloc_mem();
void				*malloc(size_t size);
void				*realloc(void *ptr, size_t size);
void				free(void *ptr);

// macros
# define PAGE_SIZE ((size_t)getpagesize())
# define META_SIZE (sizeof(t_chunk))
# define OFFSET (alignof(max_align_t) - sizeof(t_chunk)) % alignof(max_align_t)
# define ALIGN (alignof(max_align_t)) // usually 8 or 16 depending on the architecture
# define ALIGNED_SIZE(x) ((x + ((alignof(max_align_t) - x) % ALIGN))) // align the size to the nearest multiple of ALIGN

# define TINY_SIZE (1 * PAGE_SIZE)
# define SMALL_SIZE (25 * PAGE_SIZE)
# define TINY_BUCKET(x) (x <= 36)
# define SMALL_BUCKET(x) (x > 36 && x <= 1024)
# define LARGE_BUCKET(x) (x > 1024)

# define GET_MEM_POINTER(x) ((void*)((char*)(t_chunk*)(x + 1) + OFFSET)) // show allocated mem pointer from the chunk pointer
# define GET_CHUNK_POINTER(x) ((t_chunk*)((char*)(((t_chunk*)x) - 1) - OFFSET)) // show chunk (header) pointer from the allocated mem pointer

# define STDOUT 1
# define MIN(x, y) ((x) < (y) ? (x) : (y))

# ifdef __DEBUG__
#  define STDERR 2
#  define DEBUG_LOG(fmt, ...) nomalloc_log(STDERR, fmt, __VA_ARGS__)
#  define DEBUG_PRINT(str) nomalloc_log(STDERR, str)
# else
#  define DEBUG_LOG(fmt, ...) do {} while (0)
#  define DEBUG_PRINT(str) do {} while (0)
# endif

# define MMAP_PROTECTIONS (PROT_READ | PROT_WRITE)
# define MMAP_FLAGS (MAP_ANON | MAP_PRIVATE)
# define CHUNK_IS_FREE(c) (!(c->meta & ALLOCED))

# define ULL unsigned long long
# define NOT_LONG_BOUNDARY(X) (((long)X & (sizeof (long) - 1)))
# define UNALIGNED(X, Y) (NOT_LONG_BOUNDARY(X) | NOT_LONG_BOUNDARY(Y))
# define BIGBLOCKSIZE (sizeof (long) << 2)
# define LITTLEBLOCKSIZE (sizeof (long))
# define TOO_SMALL(LEN) ((LEN) < BIGBLOCKSIZE)
# define DEC "0123456789"
# define HEX "0123456789ABCDEF"

#endif

