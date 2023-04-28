#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "os_list.h"

#define MAX_TASK 10000
#define MAX_THREAD 8

int sum;

int test_processing_done(os_threadpool_t *tp)
{
    return sum == MAX_TASK;
}

void add_one(void *arg)
{
    // printf("%d--\n", sum);
    sum += 1;
}

void test_threadpool()
{
    os_threadpool_t *tp = threadpool_create(MAX_TASK, MAX_THREAD);
    if (tp == NULL) {
        printf("Failed to create thread pool.\n");
        return;
    }

    sum = 0;
    for (int i = 0; i < MAX_TASK; i++) {
        os_task_t *t = task_create((void *) &sum, &add_one);
        add_task_in_queue(tp, t);
    }

    threadpool_stop(tp, test_processing_done);
    printf("%d\n", sum);
}

int main(int argc, char *argv[])
{
    test_threadpool();
    return 0;
}

