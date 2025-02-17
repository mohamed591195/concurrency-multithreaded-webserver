#include "tpool.h"
#include "linked_list.h"
#include <stdlib.h>
#include <stdio.h>

// data structure used for FIFO policy
// Linked LIst node
struct ll_node {
    tpool_work_t *work;
    struct ll_node *next;
};

struct linked_list {
    ll_node_t *head;
    ll_node_t *tail;
};

void *create_llist() {
    linked_list_t *llist = calloc(1, sizeof(linked_list_t));
    if (llist == NULL) {
        fprintf(stderr, "memory allocation failed at create_llist\n");
        return NULL;
    }

    llist->head = llist->tail = NULL;
    return llist;
}

void destroy_llist(void *context) {
    linked_list_t *llist = (linked_list_t *) context;
    if (llist == NULL) return;

    // tpool_work_t *work = llist->head;
    ll_node_t *current_node = llist->head;

    while (current_node != NULL) {
        ll_node_t *next = current_node->next;
        tpool_work_destroy(current_node->work);
        free(current_node);
        current_node = next;
    } 

    free(llist);
}

tpool_work_t *llist_work_get(void *context) {
    linked_list_t *llist = (linked_list_t *) context;
    if (llist == NULL) return NULL;

    // work_first
    if (llist->head == NULL) return NULL;

    ll_node_t *node = llist->head;
    
    //extracting work;
    tpool_work_t *work = node->work;

    llist->head = llist->head->next; 
    if (llist->head == NULL) llist->tail = NULL;
    
    free(node); //freeing node memory    
    return work;
}

bool llist_work_add(void *context, tpool_work_t *work) {
    linked_list_t *llist = (linked_list_t *) context;

    // we make sure that we'r not trying to add work while destroy_pool is being processed concurrently
    if (llist == NULL || work == NULL) return false;

    ll_node_t *node = malloc(sizeof(ll_node_t));

    if (!node) {
        fprintf(stderr, "memory allocation failed at llist_work_add\n");
        return false;
    }

    node->work = work;
    node->next = NULL;

    if (llist->head == NULL) llist->head = node;
    else {
        llist->tail->next = node;
    }
    llist->tail = node;

    return true;
}


bool llist_empty(void *context) {
    linked_list_t *llist = (linked_list_t *) context;
    if (llist == NULL || llist->head == NULL) return true;
    return false;
}
