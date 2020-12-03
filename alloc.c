#include <unistd.h>
#include "structs.h"

static mem_t *first = HEAP_START;
static mem_t *map;
static size_t bl;

void *m_malloc(size_t qu) {
    mem_t *iter = first;
    mem_t *next;
    mem_t *block;
    mem_t *new_b;
    size_t cap;
    if (qu < BLOCK_MIN_SIZE)
        qu = BLOCK_MIN_SIZE;
    block = find_suitable(iter, qu);
    if (block == NULL) return NULL;
    next = block->next;
    cap = block->capacity;
    bl = (size_t) block;
    block->next = (mem_t *) (bl + sizeof(mem_t) + qu);
    block->capacity = qu;
    block->is_free = 0;
    new_b = block->next;
    new_b->next = next;
    new_b->is_free = 1;
    new_b->capacity = cap - qu - sizeof(mem_t);

    return (void *) (bl + sizeof(mem_t));
}

mem_t *find_suitable(mem_t *iter, size_t qu) {
    size_t i = 0;
    void *ptr;
    while (((!iter->is_free) || (iter->capacity < qu + sizeof(mem_t) + BLOCK_MIN_SIZE)) && (iter->next)) {
        iter = iter->next;
    }
    if ((iter->next == NULL) && ((!iter->is_free) || (iter->capacity < qu + sizeof(mem_t) + BLOCK_MIN_SIZE))) {
        i = (size_t) iter + sizeof(mem_t) + iter->capacity;
        ptr = (void *) i;
        map = (mem_t *) map_after(ptr, qu + 2 * sizeof(mem_t) + BLOCK_MIN_SIZE);

        if (map != NULL) {
            iter->next = map;
            return map;
        } else { map = (mem_t *) map_evr(qu + 2 * sizeof(mem_t) + BLOCK_MIN_SIZE); }
        if (map != NULL) {
            iter->next = map;
            return map;
        } else {
            return NULL;
        }
    }
    return iter;
}

void *map_after(void *point, size_t size) {
    size_t x = (size_t) point;
    long mem = sysconf(_SC_PAGESIZE);
    map = (mem_t *) mmap((void *) (x + mem - x % mem), size, PROT_READ | PROT_WRITE,
                         MAP_FIXED | MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if ((size_t) map == -1) { return NULL; }
    map->next = NULL;
    map->is_free = 1;
    map->capacity = cast(size) - sizeof(mem_t);
    return map;
}

void *map_evr(size_t size) {
    map = (mem_t *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if ((size_t) map == -1) { return NULL; }
    map->next = NULL;
    map->is_free = 1;
    map->capacity = cast(size) - sizeof(mem_t);
    return map;
}

void *heap_init(size_t init_size) {
    map = (mem_t *) mmap(HEAP_START, init_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if ((size_t) map == -1) { return NULL; }
    map->next = NULL;
    map->is_free = 1;
    map->capacity = cast(init_size) - sizeof(mem_t);
    return map;
}

size_t cast(size_t qu) {
    if (qu % 4096 == 0) { return qu; }
    return (qu / 4096 + 1) * 4096;
}


void memalloc_debug_struct_info(FILE *f,
                                struct mem_t const *const address) {
    size_t i;
    fprintf(f,
            "start: %p\nsize: %lu\nis_free: %d\n",
            (void *) address,
            address->capacity,
            address->is_free);
    for (i = 0;
         i < address->capacity;
         i++)
        fprintf(f, "%hX",
                ((char *) address)[sizeof(struct mem_t) + i]);
    putc('\n', f);
}

void memalloc_debug_heap(FILE *f, struct mem_t const *ptr) {
    for (; ptr; ptr = ptr->next)
        memalloc_debug_struct_info(f, ptr);
}

void m_free(void *ptrd) {
    int i;
    size_t pointer = (size_t) ptrd - sizeof(mem_t);
    mem_t *ptr = (mem_t *) pointer;
    ptr->is_free = 1;
    while ((ptr->next == (void *) ((size_t) ptrd + ptr->capacity)) && (ptr->next->is_free)) {
        ptr->capacity = ptr->capacity + sizeof(mem_t) + ptr->next->capacity;
        ptr->next = ptr->next->next;
    }
    for (i = 0; i < ptr->capacity; i++) {
        *((unsigned char *) ptrd + i) = 0;
    }
}