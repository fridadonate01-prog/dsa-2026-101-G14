#include <stdio.h>
#include <stdlib.h>
#include "route.h"

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