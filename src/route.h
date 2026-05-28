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

void append_street(PathNode** head, Street* street);
void free_path(PathNode* head);
PathNode* find_route(Street* origin_street, Street* dest_street, Street* all_streets);

double toRadians(double degree);
void latlon_to_xy(double lat_ref, double lon_ref, double lat, double lon, double *x, double *y);
int get_turn_direction(Street* s1, Street* s2);

#endif