/*thread_pool.c - workers that pull from the task_queue.c container concurrently*/

#include <stdlib.h>
#include "thread_pool.h"

void* worker_thread(void* arg){
    ThreadPool* pool = (ThreadPool*)arg;

    while(pool->shutdown == 0 || task_queue_size(&pool->queue)){
        Task* task = task_queue_dequeue(&pool->queue);
        task->function(task->arg);
        free(task);
    }

    return NULL;
}

void thread_pool_init(ThreadPool* pool, int num_workers, int queue_capacity){
    pool->num_workers = num_workers;
    pool->shutdown = 0;
    task_queue_init(&pool->queue, queue_capacity);

    for(int i = 0; i < num_workers; i++){
        // create each worker thread, passing pool as the argument
        pthread_create(&pool->workers[i], NULL, worker_thread, pool);
    }
}

void thread_pool_submit(ThreadPool* pool, Task* task){
    task_queue_enqueue(&pool->queue, task);
}

void thread_pool_destroy(ThreadPool* pool){
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->queue.not_empty);

    for(int i = 0; i < pool->num_workers; i++){
        pthread_join(pool->workers[i], NULL);
    }
}