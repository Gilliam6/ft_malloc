ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

UNAME_S   := $(shell uname -s)
LIBFT_DIR := libft
NAME      = libft_malloc_$(HOSTTYPE).so
SRCS      = src/malloc.c src/free.c src/chunks.c src/util.c src/realloc.c src/show_alloc_mem.c src/log.c
OBJS      = $(SRCS:.c=.o)
CC        = cc
AR        = ar rc
ifeq ($(UNAME_S),Darwin)
	DYLIB = -shared -exported_symbols_list exported.syms
else
	DYLIB = -shared
endif
CFLAGS    = -Wall -Wextra -Werror -Wpedantic -fPIC -g

# Первая цель = цель по умолчанию при вызове make (без аргументов)
all: $(NAME)

$(LIBFT_DIR)/.fpic: $(wildcard $(LIBFT_DIR)/ft_*.c)
	$(MAKE) -C $(LIBFT_DIR) CFLAGS="-Wall -Wextra -Werror -fPIC"
	@touch $@

$(NAME): $(OBJS) $(LIBFT_DIR)/.fpic
	$(CC) $(CFLAGS) $(DYLIB) $(OBJS) $(wildcard $(LIBFT_DIR)/*.o) -o $@
	ln -sf $(NAME) libft_malloc.so

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += -D__DEBUG__
debug: fclean $(NAME)

clean:
	rm -f $(OBJS)
	rm -f $(LIBFT_DIR)/.fpic

fclean: clean
	rm -f $(NAME) libft_malloc.so
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re debug
