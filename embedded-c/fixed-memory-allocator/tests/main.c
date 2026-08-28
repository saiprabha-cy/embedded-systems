#include <stdio.h>
#include "memory_pool.h"

int main(void)
{
    void *a;
    void *b;
    void *c;
    void *d;
    void *e;

    pool_init();

    printf("Free blocks: %zu\n", pool_free_count());

    a = pool_alloc();
    b = pool_alloc();
    c = pool_alloc();
    d = pool_alloc();

    printf("Free blocks: %zu\n", pool_free_count());

    /* Pool is full */
    e = pool_alloc();

    /* Release block 1 */
    pool_free(b);

    printf("Free blocks: %zu\n", pool_free_count());

    /* Reuse the released block */
    e = pool_alloc();

    printf("Free blocks: %zu\n", pool_free_count());

    (void)a;
    (void)c;
    (void)d;
    (void)e;

    return 0;
}