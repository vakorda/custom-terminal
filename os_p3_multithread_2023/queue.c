//OS-P3 2022-2023

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"



//To create a queue
queue* queue_init(int size) {
    queue* q = malloc(sizeof(queue));
    q -> buffer = malloc(sizeof(element)*size);
    q -> head = 0;
    q -> tail = 0;
    q -> filled = 0;
    for(int i=0; i < size; i++) {
        element e;
        q -> buffer[i] = e;
    }
    q -> size = size;
    return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element* x) {
    if(q -> filled != q -> size) {
        if (q -> filled == 0) {
            q -> buffer[q -> tail] = *x;
        } else {
            q -> buffer[q -> tail + 1] = *x;
        }
        q -> tail = (q -> tail + 1) % q -> size;
        q -> filled = (q -> tail - q -> head) % q -> size;
        return 0;
    } else {
        return -1;
    }
}


// To Dequeue an element.
struct element* queue_get(queue *q) {
    if (q -> filled != 0) {
        element *head = &q -> buffer[q -> head];
        q -> head = (q -> head + 1) % q -> size;
        q -> filled = (q -> tail - q -> head) % q -> size;
        return head;
    }
    return (element *) (- 1);
}


//To check queue state
int queue_empty(queue *q){
    return !(q -> filled == 0);
}

int queue_full(queue *q){
    return !(q -> filled == q -> size);
}

//To destroy the queue and free the resources
int queue_destroy(queue *q){
    free(q -> buffer);
    free(q);
    return 0;
}
