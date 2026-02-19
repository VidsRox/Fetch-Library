#include "task_queue.h"

#define MAX_WORKERS 8

typedef struct{
    pthread_t workers[MAX_WORKERS];
    TaskQueue queue;
    int num_workers;
    int shutdown;
} ThreadPool;

void thread_pool_init(ThreadPool* pool, int num_workers, int queue_capacity);

void thread_pool_submit(ThreadPool* pool, Task* task);

void thread_pool_destroy(ThreadPool* pool);