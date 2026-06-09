#ifndef THREAD_PULL_H
#define THREAD_PULL_H

typedef struct Task {
    void (*func)(void*);
    void* arg;
    struct Task* next;
} Task;

#endif