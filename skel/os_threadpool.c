#include "os_threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* === TASK === */

/* Creates a task that thread must execute */
os_task_t *task_create(void *arg, void (*f)(void *))
{
    // Alloc memory for a new task
    os_task_t *t = (os_task_t *) malloc(sizeof(os_task_t));
    if (t == NULL) {
        printf("[ERROR]: Can't allocate memory for `task`");
        return NULL;
    }

    // Extract the `arguments` and the `function`
    t->argument = arg;
    t->task = f;

    return t;
}

/* Add a new task to threadpool task queue */
void add_task_in_queue(os_threadpool_t *tp, os_task_t *t)
{
<<<<<<< HEAD
    // Create a new node and assign the given task `t`
=======
    // Create a new node in the queue
>>>>>>> bothering-lock
    os_task_queue_t *new_node = (os_task_queue_t *) malloc(sizeof(os_task_queue_t));
    if (new_node == NULL) {
        printf("[ERROR]: Can't allocate memory for task queue node `new_node`");
        return;
    }

    // Set the nodes' task and next node
    pthread_mutex_lock(&tp->taskLock);
    new_node->task = t;
    new_node->next = NULL;

    // Add the task to the begging of the queue
    if (tp->tasks == NULL) {
        tp->tasks = new_node;
    } else {
        new_node->next = tp->tasks;
        tp->tasks = new_node;
    }
<<<<<<< HEAD

=======
>>>>>>> bothering-lock
    pthread_mutex_unlock(&tp->taskLock);
}

/* Get the head of task queue from threadpool */
os_task_t *get_task(os_threadpool_t *tp)
{
    pthread_mutex_lock(&tp->taskLock);

    // Check if tasks' queue is empty
    if (tp->tasks == NULL) {
        pthread_mutex_unlock(&tp->taskLock);
        return NULL;
    }

    // Get the head of the queue
    os_task_queue_t *head = tp->tasks;
    os_task_t *task = head->task;

    // Remove the head of the queue
    tp->tasks = tp->tasks->next;
    free(head);

    pthread_mutex_unlock(&tp->taskLock);

    return task;
}

/* === THREAD POOL === */

/* Wrapper of malloc */
os_threadpool_t *_os_threadpool_create()
{
    // Alloc memory for a new threadpool
    os_threadpool_t *tp = (os_threadpool_t *) malloc(sizeof(os_threadpool_t));
    if (tp == NULL) {
        printf("[ERROR]: Can't allocate memory for `threadpool`");
        return NULL;
    }

    // Set some default values
    tp->should_stop = 0;
    tp->num_threads = 0;
    tp->threads = NULL;
    tp->tasks = NULL;

    // Initialize the `taskLock` mutex
    if (pthread_mutex_init(&tp->taskLock, NULL) != 0) {
        printf("[ERROR]: Can't initialize the `taskLock` mutex");
        free(tp);
        return NULL;
    }

    return tp;
}

/* Initialize the new threadpool */
os_threadpool_t *threadpool_create(unsigned int nTasks, unsigned int nThreads)
{
    // Sanity checks
    if (nTasks <= 0 || nThreads <= 0) {
        printf("[ERROR]: `nTasks` and `nThreads` must be positive integers");
        return NULL;
    }

    // Create a new threadpool using the `malloc`'s wrapper
    os_threadpool_t *tp = _os_threadpool_create();
    if (tp == NULL) {
        printf("[ERROR]: Can't create the `threadpool`");
        return NULL;
    }

    // Set the number of threads and allocate memory for them
    tp->num_threads = nThreads;
    tp->threads = (pthread_t *) malloc(nThreads * sizeof(pthread_t));
    if (tp->threads == NULL) {
        printf("[ERROR]: Can't allocate memory for `threads`");
        pthread_mutex_destroy(&tp->taskLock);
        free(tp);
        return NULL;
    }

    // Create the threads
    for (unsigned int i = 0; i < nThreads; i++) {
        if (pthread_create(&tp->threads[i], NULL, thread_loop_function, (void *) tp) != 0) {
            printf("[ERROR]: Can't create thread #%d", i);
            for (unsigned int j = 0; j < i; j++)
                pthread_cancel(tp->threads[j]);
            pthread_mutex_destroy(&tp->taskLock);
            free(tp->threads);
            return NULL;
        } 
    }

    return tp;
}

/* Loop function for threads */
void *thread_loop_function(void *args)
{
    // Get the threadpool
    os_threadpool_t *tp = (os_threadpool_t *) args;
    os_task_t *task = NULL;

    // Loop until the threadpool is stopped
    while (1) {
        // Check if the threadpool received a stop signal
        if (tp->should_stop)
            break;

        // Wait until a task is added to the queue
        while (tp->tasks == NULL && !tp->should_stop) {}
            
        // Get a task from the queue
        task = get_task(tp);

        // Execute the task
        if (task != NULL) {
            void *arg = task->argument;
            void (*f)(void *) = task->task;
            f(arg);
        }
    }

    pthread_exit(NULL);
}

/* Stop the thread pool once a condition is met */
void threadpool_stop(os_threadpool_t *tp, int (*processingIsDone)(os_threadpool_t *))
{
    // Block until all stop signal is received
    while (!processingIsDone(tp)) {}

    // Notify the threads that they should stop
    tp->should_stop = 1;

    // Join all the worker threads
    for (unsigned int i = 0; i < tp->num_threads; i++)
        pthread_join(tp->threads[i], NULL);

<<<<<<< HEAD
    os_task_queue_t *curr_node = tp->tasks;
    while (curr_node != NULL) {
        os_task_queue_t *tmp = curr_node;
        curr_node = curr_node->next;
        free(tmp->task->argument);
        free(tmp->task);
        free(tmp);
    }

    free(tp->threads);
=======
    // Destroy the mutex
>>>>>>> bothering-lock
    pthread_mutex_destroy(&tp->taskLock);
    free(tp);
}
