#include "memory_pool.h"
#include <stdio.h>
#include <stdint.h>

static uint8_t memory_pool[BLOCK_SIZE * BLOCK_COUNT];
static uint8_t block_used[BLOCK_COUNT];

void pool_init(void)
{
    for (size_t i = 0; i < BLOCK_COUNT; i++)
    {
        block_used[i] = 0;
    }

    printf("Memory pool initialized\n");
}

void *pool_alloc(void)
{
    for (size_t i = 0; i < BLOCK_COUNT; i++)
    {
        if (block_used[i] == 0)
        {
            block_used[i] = 1;

            printf("Allocated block %zu\n", i);

            return &memory_pool[i * BLOCK_SIZE];
        }
    }

    printf("Allocation failed: pool full\n");

    return NULL;
}

void pool_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    for (size_t i = 0; i < BLOCK_COUNT; i++)
    {
        if (ptr == &memory_pool[i * BLOCK_SIZE])
        {
            block_used[i] = 0;

            printf("Freed block %zu\n", i);

            return;
        }
    }

    printf("Invalid pointer\n");
}

size_t pool_free_count(void)
{
    size_t count = 0;

    for (size_t i = 0; i < BLOCK_COUNT; i++)
    {
        if (block_used[i] == 0)
        {
            count++;
        }
    }

    return count;
}