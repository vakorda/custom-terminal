#ifndef HEADER_FILE
#define HEADER_FILE


typedef struct element {
    char *operation;
} element;

typedef struct queue {
    int size;
    element *buffer;
    int head;
    int tail;
    int filled;
} queue;

queue* queue_init (int size);
int queue_destroy (queue *q);
int queue_put (queue *q, struct element* elem);
struct element * queue_get(queue *q);
int queue_empty (queue *q);
int queue_full(queue *q);
int print_elems(queue* q);

#endif
