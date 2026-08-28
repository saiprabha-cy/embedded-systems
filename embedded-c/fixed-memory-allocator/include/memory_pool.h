#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <stddef.h>

/* Configuration */
#define BLOCK_SIZE  16
#define BLOCK_COUNT 4

/* Initialize the memory pool */
void pool_init(void);

/* Allocate one fixed-size block */
void *pool_alloc(void);

/* Return a block to the pool */
void pool_free(void *ptr);

/* Check how many blocks are currently free */
size_t pool_free_count(void);

#endif /* MEMORY_POOL_H */