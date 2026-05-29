#ifndef ROUTE_H
#define ROUTE_H

#include "find.h"

typedef struct QueueNode {
    StreetNode* path;         // The entire street list 
    struct QueueNode* next; // Pointer to the next path waiting in the queue
} QueueNode;

typedef struct {
    QueueNode* front;       // Where we dequeue
    QueueNode* rear;        // Where we enqueue
} Queue;

// DECLARATIONS
void init_queue(Queue* q);
void enqueue(Queue* q, StreetNode* path);
StreetNode* dequeue(Queue* q);
int is_queue_empty(Queue* q);

void append_street(StreetNode** head, Street* street);
void free_path(StreetNode* head);
StreetNode* find_route(Street* origin_street, Street* dest_street, Street* all_streets);

double toRadians(double degree);
void latlon_to_xy(double lat_ref, double lon_ref, double lat, double lon, double *x, double *y);
int get_turn_direction(Street* s1, Street* s2);

void print_route_directions(StreetNode* route);
#endif