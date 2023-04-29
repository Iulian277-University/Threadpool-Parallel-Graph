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

static int sum;
static os_graph_t *graph;
os_threadpool_t *tp;

pthread_mutex_t sum_lock;
pthread_mutex_t visited_lock;

int is_processing_done(os_threadpool_t *tp)
{
    return tp->tasks == NULL;
}

void process_node(void *arg)
{
    int node_id = *(int *)arg;
    // printf("Processing node %d\n", node_id);

    // Do nothing if the node was already visited (by another thread)
    pthread_mutex_lock(&visited_lock);
    if (graph->visited[node_id]) {
        pthread_mutex_unlock(&visited_lock);
        return;
    }

    // Mark the node as visited
    graph->visited[node_id] = 1;
    pthread_mutex_unlock(&visited_lock);

    // Add the node's info to the sum
    pthread_mutex_lock(&sum_lock);
    sum += graph->nodes[node_id]->nodeInfo;
    pthread_mutex_unlock(&sum_lock);

    // Start a new task for each neighbour
    for (int i = 0; i < graph->nodes[node_id]->cNeighbours; ++i) {
        int *neighbour_id = malloc(sizeof(int));
        *neighbour_id = graph->nodes[node_id]->neighbours[i];

        // Check if the neighbour was already visited
        pthread_mutex_lock(&visited_lock);
        if (graph->visited[*neighbour_id]) {
            pthread_mutex_unlock(&visited_lock);
            continue;
        }
        pthread_mutex_unlock(&visited_lock);

        // Start a new task for the neighbour and add it to the thread pool
        os_task_t *t = task_create((void *) neighbour_id, &process_node);
        add_task_in_queue(tp, t);

        // Free the memory allocated for the neighbour id
        free(neighbour_id);
    }
}

int main(int argc, char *argv[])
{
	FILE *input_file;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_file = fopen(argv[1], "r");
	if (input_file == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	graph = create_graph_from_file(input_file);
	if (graph == NULL) {
		fprintf(stderr, "[Error] Can't read the graph from file\n");
		return -1;
	}

    tp = threadpool_create(MAX_TASK, MAX_THREAD);
    if (tp == NULL) {
        printf("[ERROR]: Can't to create the thread pool.\n");
        return -1;
    }

    // Initialize the locks
    pthread_mutex_init(&sum_lock, NULL);
    pthread_mutex_init(&visited_lock, NULL);

    // Start a new task from each node (the graph is not connected)
    // If the graph is connected, we can start from a single node
    // and it will expand to all the nodes in the graph
    for (int i = 0; i < graph->nCount; i++) {
        int *node_id = malloc(sizeof(int));
        *node_id = i;
        os_task_t *t = task_create((void *) node_id, &process_node);
        add_task_in_queue(tp, t);
        free(node_id);
    }

    // Wait for all the tasks to finish
    threadpool_stop(tp, &is_processing_done);

    // Destroy the locks
    pthread_mutex_destroy(&sum_lock);
    pthread_mutex_destroy(&visited_lock);

    // Print the computed sum
	printf("%d", sum);
	return 0;
}