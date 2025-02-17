#ifndef __SCHEDULERS_H__
#define __SCHEDULERS_H__
#include "tpool.h"
#include "heap.h"
#include "linked_list.h"
// #include <stdlib.h>

/////////////////////////////////////////////////////// Implementation using Strategy pattern ///////////////////////////////////////////

// 1. Strategy Interface: strategy_t defined and exposed in the public header file
typedef struct strategy {
    tpool_work_t* (*get_next_work)(void *context);
    bool (*add_work) (void *context, tpool_work_t *work); 
    void* (*create_data) ();
    void (*destroy_data) (void *data);
    bool (*data_empty) (void *data);
} strategy_t;

// 2. concrete strategies
extern strategy_t linked_list_strategy;
extern strategy_t heap_strategy;

// 3. context: store object
typedef struct store {
    strategy_t *strategy;
    void *data;
} store_t;


store_t *create_store(strategy_t *strategy);
void destroy_store(store_t *store);

#endif