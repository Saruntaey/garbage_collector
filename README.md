# Garbage Collector in C

This project manages your memory allocation. So you do not need to free the unused memory yourself. The implementation details can be read in [the article](https://dev.to/taey/garbage-collector-implementation-in-c-4iol)

## Usage
```c
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

  stud_lst = NULL;
  gc_clean();
  return 0;
}
```

## Limitation
- Do not work for packed structs.
- The address of allocated memory should be stored as it is (e.g., no XOR for making a singly linked list able to traverse both ways)
- Only tested on X86_64 and the GCC compiler
