#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "route.h"

#define EARTH_RADIUS 6371.0

// QUEUE OF STREET LISTS' FUNCTIONS
void init_queue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
}

int is_queue_empty(Queue* q) {
    return (q->front == NULL);
}

// Helper function that makes an exact copy of a street list so that branching paths don't tamgle up their histories.
PathNode* clone_path(PathNode* head) {
    if (head == NULL) return NULL;

    // Allocate memory for the first node of the cloned path
    PathNode* new_head = (PathNode*)malloc(sizeof(PathNode));
    new_head->street = head->street;
    new_head->next = NULL;

    PathNode* current = head->next; //Pointer that walks through the old list to see what streets we need to copy next.
    PathNode* tail = new_head;      // Keeps track of the very end of the new cloned list

    // Loop through the rest of the original street list and copy each element
    while (current != NULL) {
        PathNode* new_node = (PathNode*)malloc(sizeof(PathNode));
        new_node->street = current->street;
        new_node->next = NULL;

        tail->next = new_node;
        tail = new_node;

        current = current->next;
    }

    return new_head;
}

// Add a complete journey (street list) to the back of the queue
void enqueue(Queue* q, PathNode* path) {
    QueueNode* new_queue_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (new_queue_node == NULL) {
        printf("Error: Out of memory inside enqueue!\n");
        return;
    }

    new_queue_node->path = path;  // Assign the street list to this queue slot
    new_queue_node->next = NULL;

    if (q->rear == NULL) {
        q->front = new_queue_node;
        q-> rear = new_queue_node;
        return;
    }

    q->rear->next = new_queue_node;
    q->rear = new_queue_node;
}

// Take a journey (street list) out from the FRONT of the queue
PathNode* dequeue(Queue* q) {
    if (is_queue_empty(q)) {
        return NULL;
    }

    QueueNode* temp = q->front;
    PathNode* path = temp->path;    // Grab the street list pointer

    q->front = q->front->next;

    //If front front is now NULL, the queue is completely empty
    if (q->front == NULL) {
        q-> rear = NULL;
    }

    free(temp);
    return path;
}


// BFS ALGORITHM THE GRAPH FUNCTIONS
// Helper: Append a street to the end of a street list (PathNode chain)
void append_street(PathNode** head, Street* street) {
    PathNode* new_node = (PathNode*)malloc(sizeof(PathNode));
    if (!new_node) return;
    new_node->street = street;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
        return;
    }

    PathNode* curr = *head;
    while(curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_node;
}

// Helper: Free an allocated street list path
void free_path(PathNode* head) {
    while (head != NULL) {
        PathNode* temp = head;
        head = head->next;
        free(temp);
    }
}

// The Pathfinding Function
PathNode* find_route(Street* fromStreet, Street* toStreet, Street* all_streets) {
    // Edge case check
    if (!fromStreet || !toStreet || !all_streets) {
        return NULL;
    }

    // *Optimization BFS: Reset all street visited flags to 0 (we eliminate the linked list, visited)
    Street* reset_curr = all_streets;
    while (reset_curr != NULL) {
        reset_curr->visited = 0;
        reset_curr = reset_curr->next;
    }

    // Create an empty queue of street lists, Q
    Queue Q;
    init_queue(&Q);

    // Create a street list [fromStreet], initial_path
    PathNode* initial_path = NULL;
    append_street(&initial_path, fromStreet);

    // Enqueue initial_path intp Q
    enqueue(&Q, initial_path);

    PathNode* final_route = NULL;   // To track if we successfully find the path

    while (!is_queue_empty(&Q)) {
        
        PathNode* path = dequeue(&Q);

        // current_street = path[-1] (Find the last element of the list)
        PathNode* tracking = path;
        while (tracking->next != NULL) {
            tracking = tracking->next;
        }
        Street* current_street = tracking->street;

        // If current_street == to Street: return path
        if (current_street == toStreet) {
            final_route = path;
            break;
        }

        // *Optimization BFS: O(1) check instead of looping through a visited list
        if (!current_street->visited) {
            current_street->visited = 1;    // Marked as visited instantly

            // For connected_street in intersections_graph[current_street.to_intersection_id]:
            // We scan the map database to find any street connected to current_Street
            Street* connected_street = all_streets;
            while (connected_street != NULL) {

                //Connection rule: they share a starting or ending intersection ID
                int is_connected = (connected_street != current_street) && (connected_street->start.id == current_street->start.id || connected_street->start.id == current_street->end.id || connected_street->end.id == current_street->start.id || connected_street->end.id == current_street->end.id);

                // *Optimization BFS: O(1) check for neighbor
                if (is_connected) {
                    if (!connected_street->visited) {
                        PathNode* new_path = clone_path(path);
                        append_street(&new_path, connected_street);
                        enqueue(&Q, new_path);
                    }
                }
                connected_street = connected_street->next;  // To keep scanning map
            }
        }

        // If this wasn't our target path, free its temporary list copy to avoid memory leaks
        if (final_route != path) {
            free_path(path);
        }
    }

    // If we exited early but things are still left in the queue, we clean them out
    while (!is_queue_empty(&Q)) {
        PathNode* leftover = dequeue(&Q);
        free_path(leftover);
    }

    return final_route;
}


// USAGE OF THE CROSS PRODUCT -> TURN LEFT OR RIGHT
// Helper: converts degrees to radians
double toRadians(double degree) {
    return degree * (M_PI / 180.0);
}

// Projection function
void latlon_to_xy(double lat_ref, double lon_ref, double lat, double lon, double *x, double *y) {
    double lat_ref_rad = toRadians(lat_ref);
    double dlat = toRadians(lat - lat_ref);
    double dlon = toRadians(lon - lon_ref);
    *x = EARTH_RADIUS * dlon * cos(lat_ref_rad);
    *y = EARTH_RADIUS * dlat;
}

// Function to determine turn direction
int get_turn_direction(Street* s1, Street* s2) {
    if (!s1 || !s2) {
        return 0;
    }
    
    Node A_node, B_node, C_node;
    int found_intersection = 0;

    // 1. Identify the shared intersection node 'B' and incoming node 'A' from s1
    if (s1->end.id == s2->start.id || s1->end.id == s2->end.id) {
        B_node = s1->end;   // Intersection is at the end of s1
        A_node = s1->start; // We came from the start of s1
        found_intersection = 1;
    } 
    else if (s1->start.id == s2->start.id || s1->start.id == s2->end.id) {
        B_node = s1->start; // Intersection is at the start of s1
        A_node = s1->end;   // We came from the end of s1
        found_intersection = 1;
    }
    
    if (!found_intersection) {  
        return 0;   // If they don't touch, treat as straight
    }

    // 2. Identify the outgoing node 'C' on s2
    if (s2->start.id == B_node.id) {
        C_node = s2->end;
    } else{
        C_node = s2->start;
    }

    // 3. Convert all three spherical coordinates to flat X/Y coordinates.
    // We use the intersection point 'B' as our local reference point (lat_ref, lon_ref)
    double Ax, Ay, Bx, By, Cx, Cy;

    latlon_to_xy(B_node.lat, B_node.lon, A_node.lat, A_node.lon, &Ax, &Ay);
    latlon_to_xy(B_node.lat, B_node.lon, B_node.lat, B_node.lon, &Bx, &By); // Bx and By will naturally be 0.0
    latlon_to_xy(B_node.lat, B_node.lon, C_node.lat, C_node.lon, &Cx, &Cy);

    // 4. Calculate Cross Product
    double cross_product = (Bx - Ax) * (Cy - By) - (By - Ay) * (Cx - Bx);

    // We use an epsilon check to prevent floating-point inaccuracies from misidentifying straightroads
    double epsilon = 1e-7;

    if(cross_product > epsilon) {
        return 1;   // Turn Left (> 0)
    } else if (cross_product < -epsilon) {
        return -1;  // Turn Right (< 0)
    } else {
        return 0;   // Straight
    }
}