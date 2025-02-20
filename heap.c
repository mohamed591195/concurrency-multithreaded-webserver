#include <stdio.h>
#include <stdlib.h>
// #include "tpool.h"
#include "heap.h"


#define MAX_HEAP_SIZE 1010

struct heap_node {
    int val;
    tpool_work_t *work;
};

struct min_heap {
    int size;
    heap_node_t *data[MAX_HEAP_SIZE];
};

void *create_heap(){
    min_heap_t *heap = calloc(1, sizeof(min_heap_t));
    if (heap == NULL) {
        fprintf(stderr, "Failed to allocate memory for heap \n");
        return NULL;
    }
    heap->size = 0;
    return heap;
}

void destroy_heap(void *context) {
    min_heap_t *heap = (min_heap_t *) context;
    if (heap == NULL) return;

    heap_node_t *node;

    // free all nodes directly 
    for (int i = 0; i < heap->size; ++i) {
        node = heap->data[i];
        tpool_work_destroy(node->work);
        free(node);
    }
    
    free(heap);
}

void swap(heap_node_t **a, heap_node_t **b) {
    heap_node_t* temp = *a;
    *a = *b;
    *b = temp;
}


bool heap_insert(min_heap_t *heap, heap_node_t * node) {
    if (heap == NULL) return false;

    if (heap->size >= MAX_HEAP_SIZE) {
        fprintf(stderr, "heap size exceeded the max size\n");
        return false;
    }

    heap->data[heap->size] = node;

    //percolating up
    int i = heap->size++; // post increment;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap->data[i]->val < heap->data[parent]->val) {
            swap(&heap->data[parent], &heap->data[i]);
            i = parent;
        }
        else {
            break;
        }
    }

    return true;
}


heap_node_t *heap_pop(min_heap_t *heap) {
    if (heap == NULL || heap->size == 0) return NULL;
    heap_node_t *result = heap->data[0];

    heap->data[0] = heap->data[--heap->size];

    //percolating down
    int parent = 0;
    while(parent < heap->size) {
        int left = parent * 2 + 1;
        int right = parent * 2 + 2;

        if (left < heap->size && heap->data[left]->val < heap->data[parent]->val) {
            if (right < heap->size &&  heap->data[right]->val < heap->data[left]->val) {
                swap(&heap->data[right], &heap->data[parent]);
                parent = right;
            }
            else {
                swap(&heap->data[left], &heap->data[parent]);
                parent = left;
            }
        }

        else if (right < heap->size && heap->data[right]->val < heap->data[parent]->val) {
            swap(&heap->data[right], &heap->data[parent]);
            parent = right;
        }
        else {
            break;
        }
    }
    return result;
}

heap_node_t *heap_top(min_heap_t *heap) {
    if (heap == NULL || heap->size == 0) return NULL;
    return heap->data[0];
}

////////////////////////////////////////////////// Heap abstraction ended ////////////////////////////////////////////////
///////////////////////////////////////////////// Work abstraction //////////////////////////////////////////////////////

tpool_work_t *heap_work_get(void *context) {
    min_heap_t *heap = (min_heap_t *) context;
    if (heap == NULL) return NULL;

    if (heap_top(heap) == NULL) return NULL;

    heap_node_t *node = heap_pop(heap);
    tpool_work_t *work = node->work;
    free(node);
    return work;
}

bool heap_work_add(void *context, tpool_work_t *work) {
    min_heap_t *heap = (min_heap_t *) context;
    if (heap == NULL || work == NULL) return false;

    heap_node_t *node = malloc(sizeof(heap_node_t));
    if (node == NULL) {
        fprintf(stderr, "memory allocation failed at heap_work_add\n");
        return false;
    }
    
    node->work = work; 
    node->val = work->priority;
    
    return heap_insert(heap, node);
}

bool heap_empty(void *context) {
    min_heap_t *heap = (min_heap_t *) context;
    if (heap == NULL || heap->size == 0) return true;
    return false;
}

// testing 

// int main() {
//     min_heap_t *heap = create_heap();
//     heap_insert(heap, 5);
//     heap_insert(heap, 10);
//     heap_insert(heap, 20);
//     heap_insert(heap, 1);
//     heap_insert(heap, 5);
//     heap_insert(heap, 2);

//     printf("heap top: %d\n", heap_top(heap));
//     printf("heap pop: %d\n", heap_pop(heap));
//     printf("heap top: %d\n", heap_top(heap));
//     printf("heap pop: %d\n", heap_pop(heap));
//     printf("heap top: %d\n", heap_top(heap));
//     printf("heap pop: %d\n", heap_pop(heap));
//     printf("heap pop: %d\n", heap_pop(heap));
//     printf("heap pop: %d\n", heap_pop(heap));
//     printf("heap pop: %d\n", heap_pop(heap));
//     printf("heap pop: %d\n", heap_pop(heap));
// }