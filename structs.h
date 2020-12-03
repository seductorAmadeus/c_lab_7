#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED


#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>

#define HEAP_START ((void*)0x04040000)
#define BLOCK_MIN_SIZE 8
#define MAP_ANONYMOUS 32

#pragma pack(push, 1)
typedef struct mem_t {
    struct mem_t *next;
    size_t capacity;
    int is_free;
} mem_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct test {
    int a;
    int b;
} tested;
#pragma pack(pop)

void *m_malloc(size_t query);

void m_free(void *ptrd);

void *heap_init(size_t initial_size);

void *map_evr(size_t size);

void *map_after(void *point, size_t size);

mem_t *find_suitable(mem_t *iter, size_t qu);

void *m_calloc(size_t mul, size_t size);

void *m_realloc(void *ptr, size_t new_size);

void trace(mem_t *point);

size_t cast(size_t qu);

#define DEBUG_FIRST_BYTES 4;

void memalloc_debug_struct_info(FILE *f,
                                struct mem_t const *const address);

void memalloc_debug_heap(FILE *f, struct mem_t const *ptr);

#endif
