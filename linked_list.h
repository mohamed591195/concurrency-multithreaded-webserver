#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

// #include "heap.h"
#include "tpool.h"

typedef struct linked_list linked_list_t;
typedef struct ll_node ll_node_t;

void *create_llist();
void destroy_llist(void *llist);

tpool_work_t *llist_work_get(void *llist);

bool llist_work_add(void *llist, tpool_work_t *work);
bool llist_empty(void *llist);

#endif