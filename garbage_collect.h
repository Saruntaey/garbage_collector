#ifndef __GARBAGE_COLLECT__
#define __GARBAGE_COLLECT__

#include <stddef.h>
#include <stdint.h>

extern uintptr_t stack_base;

#define ALLOC_CAP 5000
#define gc_init() \
  stack_base = (uintptr_t) __builtin_frame_address(0)

typedef struct chunk_ {
  void *addr;
  size_t size;
} chunk_t;

void *gc_calloc(size_t n, size_t size);
void gc_free(void *p);
void gc_dump_alloced();
void gc_mark();
void gc_sweep();
void gc_clean();

#endif
