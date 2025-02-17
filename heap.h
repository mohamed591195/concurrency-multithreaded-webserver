#ifndef __HEAP_H__
#define __HEAP_H__

#include "tpool.h"
// #include "linked_list.h"

typedef struct heap_node heap_node_t;
typedef struct min_heap min_heap_t;

void *create_heap();
void destroy_heap(void *heap);

tpool_work_t *heap_work_get(void *heap);

bool heap_work_add(void *heap, tpool_work_t *work);
bool heap_empty(void *heap);

#endif