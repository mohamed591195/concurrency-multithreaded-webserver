#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "io_helper.h"
#include "tpool.h"


typedef void (*thread_func_t) (void *);

struct tpool_work {
    thread_func_t func;
    void *arg;
    struct tpool_work *next;
};
typedef struct tpool_work tpool_work_t;



struct tpool {
    tpool_work_t *work_first;
    tpool_work_t *work_last;

    pthread_mutex_t work_lock;
    pthread_cond_t work_cond;
    pthread_cond_t no_work_cond;

    size_t working_cntr;
    size_t threads_cntr;

    bool stop;
};
// typedef struct tpool tpool_t;


static tpool_work_t *tpool_work_create(thread_func_t func, void *arg) {
    if (func == NULL) return NULL;

    tpool_work_t *work = malloc(sizeof(tpool_work_t));
    
    if (!work) {
        fprintf(stderr, "tpool_work_create: can't allocate memory for work");
        exit(EXIT_FAILURE);
    }

    work->func = func;
    work->arg = arg;
    work->next = NULL;

    return work;
}


static void tpool_work_destroy(tpool_work_t *work) {
    if (work != NULL) free(work);
    return;
}


static tpool_work_t *tpool_work_get(tpool_t *tm) {
    if (tm == NULL) return NULL;

    if (tm->work_first == NULL) return NULL;

    tpool_work_t *work = tm->work_first;
    tm->work_first = tm->work_first->next; 
    
    return work;
}


static void *tpool_worker(void *arg) {
    if (arg == NULL) return NULL;
    tpool_t *tm = arg;
    
    // work holder to hold the work object we fetch from the pool_mngr queue 
    tpool_work_t *work;

    while (1) {
        pthread_mutex_lock(&(tm->work_lock));

        while ((tm->work_first == NULL) && tm->stop == false){
            pthread_cond_wait(&(tm->work_cond), &(tm->work_lock));
        }

        // before even getting the work, so don't increment the workers_counter
        if (tm->stop == true) break;

        work = tpool_work_get(tm);
        
        tm->working_cntr++;

        pthread_mutex_unlock(&(tm->work_lock));

        if (work != NULL) {
            work->func(work->arg);
            tpool_work_destroy(work);
        }

        pthread_mutex_lock(&(tm->work_lock));
        tm->working_cntr--;
        
        if (tm->stop == false && tm->work_first == NULL && tm->working_cntr == 0) {
            pthread_cond_signal(&(tm->no_work_cond));
        }

        pthread_mutex_unlock(&(tm->work_lock));
    }

    // we are exiting a thread so we should decrease the number
    tm->threads_cntr--;

    // each thread exit should notify the thread_wait function 
    pthread_cond_signal(&(tm->no_work_cond));    
    pthread_mutex_unlock(&(tm->work_lock));

    return NULL;
}


tpool_t *tpool_create(size_t num) {
    if (num == 0) num = 2; // default is 2 threads

    // using calloc to initialize all the member variables to zero by default like (counters) and bool to zero 
    tpool_t *tm = calloc(1, sizeof(tpool_t));

    if (tm == NULL) {
        fprintf(stderr, "tpool_create: an error occured while memory allocation");
        exit(EXIT_FAILURE);
    }

    tm->threads_cntr = num;

    tm->work_first = NULL;
    tm->work_last = NULL;

    pthread_mutex_init(&(tm->work_lock), NULL);
    pthread_cond_init(&(tm->no_work_cond), NULL);
    pthread_cond_init(&(tm->work_cond), NULL);

    pthread_t thread;
    for (size_t i=0; i<num; ++i) {
        pthread_create(&thread, NULL, tpool_worker, tm);
        pthread_detach(thread);
    }

    return tm;
}


void tpool_destroy(tpool_t *tm) {
    if  (tm == NULL) return;

    tpool_work_t *work1, *work2;

    pthread_mutex_lock(&(tm->work_lock));

    work1 = tm->work_first;
    while(work1) {
        work2 = work1->next;
        tpool_work_destroy(work1);
        work1 = work2;
    }
    tm->work_first = NULL;
    tm->stop = true;
    // broadcasting work condition to let all threads waiting for work to move on
    pthread_cond_broadcast(&(tm->work_cond));

    //unlocking to let them resume their work 
    pthread_mutex_unlock(&(tm->work_lock));

    tpool_wait(tm);

    pthread_mutex_destroy(&(tm->work_lock));
    pthread_cond_destroy(&(tm->no_work_cond));
    pthread_cond_destroy(&(tm->work_cond));

    free(tm);
}


bool tpool_add_work(tpool_t *tm, thread_func_t func, void *arg) {
    // we make sure that we'r not trying to add work while destroy_pool is being processed concurrently
    if (tm == NULL || tm->stop) return false;

    pthread_mutex_lock(&(tm->work_lock));

    tpool_work_t *work = tpool_work_create(func, arg);

    if (!work) {
        pthread_mutex_unlock(&(tm->work_lock));
        return false;
    }

    if (tm->work_first == NULL) tm->work_first = work;
    else {
        tm->work_last->next = work;
    }
    tm->work_last = work;
    pthread_cond_broadcast(&(tm->work_cond));
    pthread_mutex_unlock(&(tm->work_lock));
    return true;
}


void tpool_wait(tpool_t *tm) {
    if (tm == NULL) return;

    pthread_mutex_lock(&(tm->work_lock));

    tm->stop = true;

    // waiting for each thread to exit
    while(1) {
        if (
            tm->work_first != NULL ||
            ((!tm->stop) && tm->working_cntr != 0) ||
            (tm->stop && tm->threads_cntr != 0)
        ) {
            pthread_cond_wait(&(tm->no_work_cond), &(tm->work_lock));
        }
        else {
            break;
        }
    }
    pthread_mutex_unlock(&(tm->work_lock));
}

