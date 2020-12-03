#include "structs.h"

int main(void) {
    void *heap = heap_init(1);

    void *block = m_malloc(1024);
    void *block2 = m_malloc(1024);
    void *block3 = m_malloc(4079);
    memalloc_debug_heap(stdout, HEAP_START);

    m_free(block);
    m_free(block2);
    m_free(block3);
    memalloc_debug_heap(stdout, HEAP_START);

    trace(heap);


    return 0;
}

void trace(mem_t *point) {
    mem_t *iter = point;
    while (iter) {
        printf("Address: %p next: %p is_free: %d capacity %lu\n", (void *) iter, (void *) iter->next, iter->is_free,
               iter->capacity);
        iter = iter->next;
    }
}
