#include <pthread.h>

typedef void (*task_fn)(void* arg);

typedef struct {
    task_fn function;
    void* arg;
} Task;

typedef struct{
    Task** tasks; //our circular buffer
    int head;
    int tail;
    int size;
    int capacity;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
} TaskQueue;

//operations
void task_queue_init(TaskQueue* q, int capacity);

void task_queue_enqueue(TaskQueue* q, Task* task);

Task* task_queue_dequeue(TaskQueue* q);

void task_queue_destroy(TaskQueue* q);

int task_queue_size(TaskQueue* q);