#include "thread_pull.h"
#include "debug.h"
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
int g_tasks_remaining = 0;
pthread_mutex_t g_pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_pool_has_tasks = PTHREAD_COND_INITIALIZER;
pthread_cond_t g_pool_finished = PTHREAD_COND_INITIALIZER;

void pool_add_task(void (*func)(void*), void* arg) {
    MH_DEBUG_PRINT("Function \"pool_add_task\" start\n");

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

    g_tasks_remaining++;

    pthread_cond_signal(&g_pool_has_tasks);

    pthread_mutex_unlock(&g_pool_mutex);

    MH_DEBUG_PRINT("Function \"pool_add_task\" end\n");
}

void pool_wait() {
    MH_DEBUG_PRINT("Function \"pool_wait\" start\n");

    pthread_mutex_lock(&g_pool_mutex);

    while (g_tasks_remaining > 0) {
        pthread_cond_wait(&g_pool_finished, &g_pool_mutex);
    }

    pthread_mutex_unlock(&g_pool_mutex);

    MH_DEBUG_PRINT("Function \"pool_wait\" end\n");
}

void* pool_worker(void* arg) {
    (void)arg;
    
    while (1) {
        pthread_mutex_lock(&g_pool_mutex);

        while(g_task_queue_head == NULL && g_stop_pool == 0) { 
            pthread_cond_wait(&g_pool_has_tasks, &g_pool_mutex);
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

        pthread_mutex_lock(&g_pool_mutex);

        g_tasks_remaining--;

        if (g_tasks_remaining == 0) {
            pthread_cond_broadcast(&g_pool_finished);
        }

        pthread_mutex_unlock(&g_pool_mutex);
        
    }

    return NULL;
}

int math_harbor_init(int num_threads) {
    MH_DEBUG_PRINT("Function \"math_harbor_init\" start\n");

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

    MH_DEBUG_PRINT("Function \"math_harbor_init\" end\n");

    return 0;
}

void math_harbor_free() {
    MH_DEBUG_PRINT("Function \"math_harbor_free\" start\n");

    pool_wait();

    pthread_mutex_lock(&g_pool_mutex);
    g_stop_pool = 1;
    pthread_mutex_unlock(&g_pool_mutex);
    pthread_cond_broadcast(&g_pool_has_tasks);

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
    pthread_cond_destroy(&g_pool_has_tasks);
    pthread_cond_destroy(&g_pool_finished);

    MH_DEBUG_PRINT("Function \"math_harbor_free\" end\n");
}