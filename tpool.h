#ifndef __TPOOL_H__
#define __TPOOL_H__

#include <stdbool.h>
#include <stddef.h>
// #include "schedulers.h"

// to avoid circular dependency from importing schedulers.h
typedef struct store store_t;

typedef void (*thread_func_t)(void *arg, char *http_first_line);

// Define tpool_work_t here (not just forward-declare)
typedef struct tpool_work {
    int priority; // for scheduling algorithms
    thread_func_t func;
    void *arg;
    char *http_first_line;
} tpool_work_t;

struct tpool;
typedef struct tpool tpool_t;

tpool_t *tpool_create(size_t num, store_t *store);
void tpool_destroy(tpool_t *tm);
bool tpool_work_add(tpool_t *tm, thread_func_t func, void *arg, int priority, char *http_first_line);
void tpool_wait(tpool_t *tm);
void tpool_work_destroy(tpool_work_t *work);

#endif /* __TPOOL_H__ */
