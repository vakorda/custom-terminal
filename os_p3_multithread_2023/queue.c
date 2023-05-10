//OS-P3 2022-2023

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"
#include <unistd.h>



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
    //printf("bef: num_elems = %d, tail= %d, head = %d, size = %d\n", q -> filled, q -> tail, q -> head, q -> size);
    fflush(stdout);
    if(q -> filled != q -> size) {
        if (q -> filled == 0) {
            q -> buffer[q -> tail] = *x;
        } else {
            q -> tail = (q -> tail + 1) % q -> size;
            q -> buffer[q -> tail] = *x;

        }
        q -> filled = q -> tail - q -> head + 1;
        if (q -> filled <= 0) {
            q -> filled = q -> size + q -> filled;
        }
        //printf("af: num_elems = %d, tail= %d, head = %d, size = %d\n", q -> filled, q -> tail, q -> head, q -> size);

        return 0;
    } else {
        fprintf(stderr, "\033[1;31mcannot put in position %d\n\033[0;38m", (q ->tail + 1) % q ->size);
        return -1;
    }
}


// To Dequeue an element.
struct element* queue_get(queue *q) {
    if (q -> filled != 0) {

        element *head = &q -> buffer[q -> head];
        if(q -> head == q ->tail) {
            q -> filled  = 0;
        } else {
            q -> head = (q -> head + 1) % q -> size;

            q -> filled = q -> tail - q -> head + 1;

            if (q -> filled <= 0) {
                q -> filled = q -> size + q -> filled;
            }
        }


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
