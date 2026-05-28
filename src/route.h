#ifndef ROUTE_H
#define ROUTE_H

#include "find.h"

typedef struct PathNode {
    Street* street;         // Pointer to a street segment from the map
    struct PathNode* next;  // Pointer to the next street in this specific path
} PathNode;

typedef struct QueueNode {
    PathNode* path;         // The entire street list 
    struct QueueNode* next; // Pointer to the next path waiting in the queue
} QueueNode;

typedef struct {
    QueueNode* front;       // Where we dequeue
    QueueNode* rear;        // Where we enqueue
} Queue;

// DECLARATIONS
void init_queue(Queue* q);
void enqueue(Queue* q, PathNode* path);
PathNode* dequeue(Queue* q);
int is_queue_empty(Queue* q);

#endif