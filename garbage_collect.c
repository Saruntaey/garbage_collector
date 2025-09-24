#include "garbage_collect.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>

extern char etext, end;
static chunk_t alloced_chunks[ALLOC_CAP] = {0};
static size_t alloced_size = 0;
static char mark[ALLOC_CAP] = {0};
uintptr_t stack_base = 0;

void *gc_calloc(size_t n, size_t size) {
  void *p;
  chunk_t c;
  int i;

  if (n*size == 0) return NULL;
  assert(alloced_size < ALLOC_CAP);
  p = calloc(n, size);
  c = (chunk_t) {.size=n*size, .addr=p};
  for (i = alloced_size - 1; i >= 0 && alloced_chunks[i].addr > p; i--) {
    alloced_chunks[i + 1] = alloced_chunks[i];
  }
  alloced_chunks[i+1] = c;
  alloced_size++;
  return p;
}

static int bs(void *p) {
  int l;
  int r;
  int mid;
  chunk_t *c;

  l = 0;
  r = alloced_size - 1;
  c = alloced_chunks;
  while (l <= r) {
    mid = l + ((r - l) >> 1);
    if (c[mid].addr == p) return mid;
    if (c[mid].addr > p) {
      r = mid - 1;
    } else {
      l = mid + 1;
    }
  }
  return -1;
}

void gc_free(void *p) {
  int i;

  if (!p) return;
  i = bs(p);
  if (i == -1) return;
  alloced_size--;
  while (i < (int) alloced_size) {
    alloced_chunks[i] = alloced_chunks[i + 1];
    i++;
  }
  free(p);
}

void gc_dump_alloced() {
  int i;

  printf("alloc (%zu)\n", alloced_size);
  for (i = 0; i < (int) alloced_size; i++) {
    printf("addr: %p, size: %zu, mark: %s\n", alloced_chunks[i].addr, alloced_chunks[i].size, mark[i] ? "true": "false");
  }
}

static int bs_lte(void *p) {
  int l;
  int r;
  int mid;
  chunk_t *c;

  l = 0;
  r = alloced_size - 1;
  c = alloced_chunks;
  while (l <= r) {
    mid = l + ((r - l) >> 1);
    if (c[mid].addr > p) {
      r = mid - 1;
    } else {
      l = mid + 1;
    }
  }
  return r;
}

static void do_mark(uintptr_t start, uintptr_t end) {
  int i;
  void **p;
  chunk_t *c;
  uintptr_t bound;

  assert(start % sizeof(void *) == 0);
  bound = start + ((end - start) / sizeof(void *) * sizeof(void*));
  for (p = (void *) start; (uintptr_t) p < bound; p++) {
    i = bs_lte(*p);
    if (i == -1) continue;
    assert(i < (int) alloced_size);
    c = &alloced_chunks[i];
    assert((uintptr_t) c->addr <= (uintptr_t) *p);
    if (!mark[i] && (uintptr_t) *p < (uintptr_t) c->addr + c->size) {
      mark[i] = 1;
      do_mark( (uintptr_t) c->addr, (uintptr_t) c->addr + c->size);
    }
  }
}

static inline uintptr_t align_down(uintptr_t addr) {
    return addr & ~(sizeof(void *) - 1);
}

static inline uintptr_t align_up(uintptr_t addr) {
    return (addr + (sizeof(void *) - 1)) & ~(sizeof(void *) - 1);
}

static void _mark_from_global_var() {
  uintptr_t _start;
  uintptr_t _end;

  _start = align_up((uintptr_t) &etext);
  _end = (uintptr_t) &end + sizeof(void *);
  assert(_start % sizeof(void *) == 0);
  assert(end % sizeof(void *) == 0);
  do_mark(_start, (uintptr_t) &alloced_chunks[0]);
  do_mark((uintptr_t) &alloced_chunks[ALLOC_CAP], _end);
}

void gc_mark() {
  uintptr_t stack_start;
  size_t i;

  stack_start = (uintptr_t) __builtin_frame_address(0);
  assert(stack_base != 0 && "gc_init() is not called");
  for (i = 0; i < alloced_size; i++) {
    mark[i] = 0;
  }
  do_mark(stack_start, stack_base + sizeof(void *));
  _mark_from_global_var();
}

void gc_sweep() {
  size_t i;
  size_t j;

  for (i = 0; i < alloced_size; i++) {
    if (!mark[i]) {
      free(alloced_chunks[i].addr);
    }
  }
  j = 0;
  for (i = 0; i < alloced_size; i++) {
    if (mark[i]) {
      alloced_chunks[j++] = alloced_chunks[i];
    }
  }
  alloced_size = j;
}


void gc_clean() {
  gc_mark();
  gc_sweep();
}
