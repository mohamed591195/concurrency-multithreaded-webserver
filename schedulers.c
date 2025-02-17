#include <stdio.h>
#include "tpool.h"
#include "heap.h"
#include "linked_list.h"
#include <stdlib.h>
#include "schedulers.h"

/////////////////////////////////////////////////////// Implementation using Strategy pattern ///////////////////////////////////////////

// 1. Strategy Interface: strategy_t defined and exposed in the public header file
// typedef struct strategy {
//     tpool_work_t* (*get_next_work)(void *context);
//     bool (*add_work) (void *context, tpool_work_t *work); 
//     void* (*create_data) ();
//     void (*destroy_data) (void *data);
//     bool (*data_empty) (void *data);
// } strategy_t;

// 2. concrete strategies
strategy_t linked_list_strategy = {
    llist_work_get, 
    llist_work_add, 
    create_llist, 
    destroy_llist, 
    llist_empty
};

strategy_t heap_strategy = {
    heap_work_get, 
    heap_work_add, 
    create_heap, 
    destroy_heap, 
    heap_empty
};

// 3. context: store object
// typedef struct store {
//     strategy_t *strategy;
//     void *data;
// } store_t;

// Life Cycle management methods 
store_t *create_store(strategy_t *strategy) {
    store_t *store = malloc(sizeof(store_t));
    if (store == NULL) {
        fprintf(stderr, "memory allocation at create_store failed\n");
        return NULL;
    }
    store->strategy = strategy;
    store->data = strategy->create_data();
    return store;
}

void destroy_store(store_t *store) {
    if (store == NULL) return;
    
    if (store->strategy->destroy_data && store->data) {
        store->strategy->destroy_data(store->data);
    }

    free(store);
}



// int main () {// Example usage:
// store_t *ll_store = create_store(&linked_list_strategy, create_llist());
// // store_t *heap_store = create_store(&heap_strategy, create_heap());
//     // ll_store->strategy->add_work(ll_store->data, work);
//     ll_store->strategy->get_next_work(ll_store->data);
//     ll_store->strategy->data_empty(ll_store->data);
// // Example usage (assuming `destroy_llist` exists):
// // destroy_store(ll_store, destroy_llist);
// // destroy_store(heap_store, destroy_heap);
// }