#ifndef THREAD_PULL_H
#define THREAD_PULL_H

typedef struct Task {
    void (*func)(void*);
    void* arg;
    struct Task* next;
} Task;

int math_harbor_init(int num_threads);

void math_harbor_free();

void pool_add_task(void (*func)(void*), void* arg);

void pool_wait();

extern int g_num_threads;

#endif