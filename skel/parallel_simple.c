#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "os_list.h"

#define MAX_TASK 100
#define MAX_THREAD 4

void print_number(void *arg)
{
    int n = *(int *)arg;
    // printf("%d\n", n);
    // return;

    // Print to res.txt
    FILE *f = fopen("res.txt", "a");
    if (f == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    float r = (float)rand() / (float)RAND_MAX;
    float sleep_time = 0.1 + r * (0.5 - 0.1);
    sleep(sleep_time * 1.2);
    fprintf(f, "%d\n", n);
    fclose(f);
}

int test_processing_done(os_threadpool_t *tp)
{
    return tp->tasks == NULL;
}

void test_threadpool()
{
    os_threadpool_t *tp = threadpool_create(MAX_TASK, MAX_THREAD);
    if (tp == NULL) {
        printf("Failed to create thread pool.\n");
        return;
    }

    int v[MAX_TASK] = {0};
    for (int i = 0; i < MAX_TASK; i++) {
        v[i] = i;
        os_task_t *t = task_create((void *) &v[i], &print_number);
        add_task_in_queue(tp, t);
    }

    // Print remaining thread pool tasks
    // os_task_queue_t *head = tp->tasks;
    // printf("Remaining tasks: ");
    // while (head != NULL) {
    //     printf("%p -> ", head->task);
    //     head = head->next;
    // }
    // printf("NULL\n");
    // fflush(stdout);

    // for (unsigned int i = 0; i < tp->num_threads; i++)
        // pthread_join(tp->threads[i], NULL);

    threadpool_stop(tp, test_processing_done);
}

int main(int argc, char *argv[])
{
    test_threadpool();
    return 0;
}

