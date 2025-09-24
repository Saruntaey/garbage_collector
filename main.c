#include "garbage_collect.h"
#include <stdio.h>

void *global_var;

typedef struct node_ node_t;

typedef struct node_ {
  node_t *left;
  node_t *right;
} node_t;

typedef struct student_ {
  int id;
  node_t node;
} student_t;

int main(void) {
  gc_init();
  node_t *stud_lst;

  global_var = gc_calloc(3, sizeof(int));
  
  stud_lst = &((student_t *) gc_calloc(1, sizeof(student_t)))->node;
  stud_lst->right = &((student_t *) gc_calloc(1, sizeof(student_t)))->node;
  stud_lst->right->right = &((student_t *) gc_calloc(1, sizeof(student_t)))->node;

  printf("global_var hold: %p\n", global_var);
  gc_mark();
  gc_dump_alloced();

  printf("---\nset stud_lst = NULL\n");
  stud_lst = NULL;
  gc_mark();
  printf("mark\n");
  gc_dump_alloced();
  gc_sweep();
  printf("sweep\n");
  gc_dump_alloced();

  return 0;
}
