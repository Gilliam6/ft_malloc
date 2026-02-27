#ifndef MALLOC_H
# define MALLOC_H

# ifndef _STDDEF_H
#  include <stddef.h>
# endif

void	nomalloc_log(int fd, const char *fmt, ...);
void	show_alloc_mem();
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);
void	free(void *ptr);
#endif