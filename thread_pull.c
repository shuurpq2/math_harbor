#include "thread_pull.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

Task* g_task_queue_head = NULL;
Task* g_task_queue_tail = NULL;

pthread_t* g_threads = NULL;
int g_num_cores = 0;
int g_num_threads = 0;

int g_stop_pool = 0;
pthread_mutex_t g_pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_pool_cond = PTHREAD_COND_INITIALIZER;

void pool_add_task(void (*func)(void*), void* arg) {
    Task* new_task = malloc(sizeof(Task));
    new_task->func = func;
    new_task->arg = arg;
    new_task->next = NULL;

    pthread_mutex_lock(&g_pool_mutex);

    if (g_task_queue_head == NULL) {
        g_task_queue_head = new_task;
        g_task_queue_tail = new_task;
    } else {
        g_task_queue_tail->next = new_task;
        g_task_queue_tail = new_task;
    }

    pthread_cond_signal(&g_pool_cond);

    pthread_mutex_unlock(&g_pool_mutex);
}

void* pool_worker(void* arg) {
    (void)arg;
    
    while (1) {
        pthread_mutex_lock(&g_pool_mutex);

        while(g_task_queue_head == NULL && g_stop_pool == 0) { 
            pthread_cond_wait(&g_pool_cond, &g_pool_mutex);
        }

        if (g_stop_pool) {
            pthread_mutex_unlock(&g_pool_mutex);
            break;
        }

        Task* task = g_task_queue_head;
        g_task_queue_head = g_task_queue_head->next;

        if (g_task_queue_head == NULL) {
            g_task_queue_tail = NULL;
        }

        pthread_mutex_unlock(&g_pool_mutex);

        task->func(task->arg);
        free(task);
        
    }

    return NULL;
}

int math_harbor_init(int num_threads) {
    pthread_mutex_lock(&g_pool_mutex);

    if (num_threads > 0) {
        g_num_threads = num_threads;
    } else {
        g_num_cores = sysconf(_SC_NPROCESSORS_ONLN);
        g_num_threads = g_num_cores;
    }
    g_threads = malloc(g_num_threads * sizeof(pthread_t));

    g_stop_pool = 0;

    for (int i = 0; i < g_num_threads; i++) {
        pthread_create(&g_threads[i], NULL, pool_worker, NULL);
    }

    pthread_mutex_unlock(&g_pool_mutex);

    return 0;
}

void math_harbor_free() {
    pthread_mutex_lock(&g_pool_mutex);
    g_stop_pool = 1;
    pthread_mutex_unlock(&g_pool_mutex);
    pthread_cond_broadcast(&g_pool_cond);

    if (g_threads != NULL) {
        for (int i = 0; i < g_num_threads; i++) {
            pthread_join(g_threads[i], NULL);
        }

        free(g_threads);
        g_threads = NULL;
    }

    pthread_mutex_lock(&g_pool_mutex);

    while (g_task_queue_head != NULL) {
        Task* temp = g_task_queue_head;
        g_task_queue_head = g_task_queue_head->next;
        free(temp);
    }
    g_task_queue_tail = NULL;

    pthread_mutex_unlock(&g_pool_mutex);
    
    pthread_mutex_destroy(&g_pool_mutex);
    pthread_cond_destroy(&g_pool_cond);
}