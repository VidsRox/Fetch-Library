#include <stdio.h>
#include <stdlib.h>
#include "task_queue.h"

void print_message(void *arg){
    printf("Task: %s\n", (char*)arg);
}

int main() {
    TaskQueue queue;
    task_queue_init(&queue, 5);

    Task* task1 = (Task*)malloc(sizeof(Task));

    task1->function = print_message;
    task1->arg = "task 1";

    Task* task2 = (Task*)malloc(sizeof(Task));

    task2->function = print_message;
    task2->arg = "task 2";

    Task* task3 = (Task*)malloc(sizeof(Task));

    task3->function = print_message;
    task3->arg = "task 3";

    task_queue_enqueue(&queue, task1);
    task_queue_enqueue(&queue, task2);
    task_queue_enqueue(&queue, task3);

    Task* result1 = task_queue_dequeue(&queue);
    result1->function(result1->arg);
    free(task1);

    Task* result2 = task_queue_dequeue(&queue);
    result2->function(result2->arg);
    free(task2);

    Task* result3 = task_queue_dequeue(&queue);
    result3->function(result3->arg);
    free(task3);

    task_queue_destroy(&queue);
}