/*task_queue.c is a thread-safe container for work. It holds tasks waiting to be executed, 
and it can be safely used by multiple threads at the same time without corrupting its own state.*/

#include <stdlib.h>
#include "task_queue.h"

void task_queue_init(TaskQueue* q, int capacity) {
    q->capacity = capacity;
    q->tasks = (Task**)malloc(sizeof(Task*) * capacity);
    q->head = 0;
    q->tail = 0;
    q->size = 0;
    
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

void task_queue_enqueue(TaskQueue* q, Task* task){
    pthread_mutex_lock(&q->lock);

    if(q->size == q->capacity){
        pthread_mutex_unlock(&q->lock);
        return;
    }

    q->tasks[q->tail] = task;              //critical section starts
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;                             //critical section ends

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

Task* task_queue_dequeue(TaskQueue* q){
    pthread_mutex_lock(&q->lock);                

    while(q->size == 0){
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    Task* task = q->tasks[q->head];         //critical section starts
    q->head = (q->head + 1) % q->capacity;
    q->size--;                              //critical section ends

    pthread_mutex_unlock(&q->lock);
    return task;
}

void task_queue_destroy(TaskQueue* q){
    free(q->tasks);
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_empty);
}

int task_queue_size(TaskQueue* q){
    pthread_mutex_lock(&q->lock);
    int size = q->size;
    pthread_mutex_unlock(&q->lock);
    return size;
}