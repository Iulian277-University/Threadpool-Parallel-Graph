#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "os_list.h"

#define MAX_TASK 10
#define MAX_THREAD 2

void print_numbers(void *arg)
{
    int n = *(int *)arg;
    printf("Number: %d\n", n);
    // fflush(stdout);
}

int test_processing_done(os_threadpool_t *tp)
{
    return 1;
}

void test_threadpool()
{
    os_threadpool_t *tp = threadpool_create(MAX_TASK, MAX_THREAD);
    if (tp == NULL) {
        printf("Failed to create thread pool.\n");
        return;
    }

    for (int i = 1; i <= MAX_TASK; i++) {
        os_task_t *t = task_create(&i, &print_numbers);
        add_task_in_queue(tp, t);
    }

    // threadpool_stop(tp, test_processing_done);
}

int main(int argc, char *argv[])
{
    test_threadpool();
    return 0;
}

