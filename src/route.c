#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "route.h"
#include "find.h"

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
StreetNode* clone_path(StreetNode* head) {
    if (head == NULL) return NULL;

    // Allocate memory for the first node of the cloned path
    StreetNode* new_head = (StreetNode*)malloc(sizeof(StreetNode));
    new_head->street = head->street;
    new_head->next = NULL;

    StreetNode* current = head->next; //Pointer that walks through the old list to see what streets we need to copy next.
    StreetNode* tail = new_head;      // Keeps track of the very end of the new cloned list

    // Loop through the rest of the original street list and copy each element
    while (current != NULL) {
        StreetNode* new_node = (StreetNode*)malloc(sizeof(StreetNode));
        new_node->street = current->street;
        new_node->next = NULL;

        tail->next = new_node;
        tail = new_node;

        current = current->next;
    }

    return new_head;
}

// Add a complete journey (street list) to the back of the queue
void enqueue(Queue* q, StreetNode* path) {
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
StreetNode* dequeue(Queue* q) {
    if (is_queue_empty(q)) {
        return NULL;
    }

    QueueNode* temp = q->front;
    StreetNode* path = temp->path;    // Grab the street list pointer

    q->front = q->front->next;

    //If front front is now NULL, the queue is completely empty
    if (q->front == NULL) {
        q-> rear = NULL;
    }

    free(temp);
    return path;
}


// BFS ALGORITHM THE GRAPH FUNCTIONS
// Helper: Append a street to the end of a street list (StreetNode chain)
void append_street(StreetNode** head, Street* street) {
    StreetNode* new_node = (StreetNode*)malloc(sizeof(StreetNode));
    if (!new_node) return;
    new_node->street = street;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
        return;
    }

    StreetNode* curr = *head;
    while(curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_node;
}

// Helper: Free an allocated street list path
void free_path(StreetNode* head) {
    while (head != NULL) {
        StreetNode* temp = head;
        head = head->next;
        free(temp);
    }
}

// The Pathfinding Function
StreetNode* find_route(Street* fromStreet, Street* toStreet, Street* all_streets, IntersectionBucket** graph, int graph_size) {
    if (!fromStreet || !toStreet || !all_streets) return NULL;

    Street* reset_curr = all_streets;
    while (reset_curr != NULL) {
        reset_curr->visited = 0;
        reset_curr = reset_curr->next;
    }

    Queue Q;
    init_queue(&Q);

    StreetNode* initial_path = NULL;
    append_street(&initial_path, fromStreet);
    fromStreet->visited = 1;
    enqueue(&Q, initial_path);

    StreetNode* final_route = NULL;
    int total_pasos = 0;
    int calles_descubiertas = 0;

    printf("\n--- [TEST DIRIGIDO] --- \n");
    printf("ORIGEN : %s (ID Inicio: %lld -> ID Fin: %lld)\n", fromStreet->street_name, (long long)fromStreet->start.id, (long long)fromStreet->end.id);
    printf("DESTINO: %s (ID Inicio: %lld -> ID Fin: %lld)\n", toStreet->street_name, (long long)toStreet->start.id, (long long)toStreet->end.id);
    printf("----------------------- \n");

    while (!is_queue_empty(&Q)) {
        total_pasos++;
        StreetNode* path = dequeue(&Q);

        StreetNode* tracking = path;
        while (tracking->next != NULL) {
            tracking = tracking->next;
        }
        Street* current_street = tracking->street;


        if (current_street == toStreet || 
            current_street->end.id == toStreet->start.id || 
            current_street->end.id == toStreet->end.id) {
            
            if (current_street != toStreet) {
                append_street(&path, toStreet);
            }
            
            printf("[TEST] ¡DESTINO ENCONTRADO EN EL PASO %d!\n", total_pasos);
            final_route = path;
            break;
        }

        long long target_id = current_street->end.id;
        int index = intersection_hash(target_id, graph_size);

        IntersectionBucket* current_bucket = graph[index];
        while (current_bucket != NULL) {
            if (current_bucket->intersection_id == target_id) break;
            current_bucket = current_bucket->next;
        }

        if (current_bucket != NULL) {
            StreetNode* neighbor_node = current_bucket->connected_streets;
            while (neighbor_node != NULL) {
                Street* connected_street = neighbor_node->street;
                if (connected_street != current_street) {
                    if (!connected_street->visited) {
                        connected_street->visited = 1; 
                        calles_descubiertas++;
                        StreetNode* new_path = clone_path(path);
                        append_street(&new_path, connected_street);
                        enqueue(&Q, new_path);
                    }
                }
                neighbor_node = neighbor_node->next; 
            }
        }

        if (final_route != path) {
            free_path(path);
        }
        
        if (total_pasos % 10000 == 0) {
            printf("[TEST] %d calles analizadas. El BFS sigue buscando...\n", total_pasos);
        }
    }

    printf("\n[TEST RESULTADO] Busqueda terminada.\n");
    printf("- Pasos totales (Calles extraidas de la cola): %d\n", total_pasos);
    printf("- Total de calles unicas exploradas en la ciudad: %d\n", calles_descubiertas);

    while (!is_queue_empty(&Q)) {
        StreetNode* leftover = dequeue(&Q);
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
void print_route_directions(StreetNode* route) {
    if (route == NULL) {
        printf("\nSorry, no route could be found between these locations.\n");
        return;
    }
    printf("\n--- ROUTE ---\n");

    StreetNode* curr = route;
    int step = 1;

    // 1. Arrancamos con la primera calle
    char* current_name = curr->street->street_name;
    double accumulated_length = curr->street->length;
    Street* last_segment_of_current_street = curr->street;

    curr = curr->next;

    // Acumulamos todos los tramos que sigan siendo de la primera calle
    while (curr != NULL && strcmp(curr->street->street_name, current_name) == 0) {
        accumulated_length += curr->street->length;
        last_segment_of_current_street = curr->street;
        curr = curr->next;
    }

    // Imprimimos el paso inicial ya con todos sus metros sumados
    printf("%d. Start at %s and continue for %.2fm\n", step++, current_name, accumulated_length);

    // 2. Procesamos el resto de las calles
    while (curr != NULL) {
        Street* first_segment_of_new_street = curr->street;
        char* new_name = first_segment_of_new_street->street_name;
        double new_accumulated_length = first_segment_of_new_street->length;

        // Calculamos el giro usando el cambio de calle exacto
        int turn = get_turn_direction(last_segment_of_current_street, first_segment_of_new_street);

        // Avanzamos y acumulamos los metros de esta nueva calle
        Street* current_segment = first_segment_of_new_street;
        curr = curr->next;
        
        while (curr != NULL && strcmp(curr->street->street_name, new_name) == 0) {
            new_accumulated_length += curr->street->length;
            current_segment = curr->street; // Guardamos este tramo por si es el último
            curr = curr->next;
        }

        // Imprimimos la instrucción ya agrupada
        if (turn == 1) {
            printf("%d. Turn left to %s and continue for %.2fm\n", step++, new_name, new_accumulated_length);
        } else if (turn == -1) {
            printf("%d. Turn right to %s and continue for %.2fm\n", step++, new_name, new_accumulated_length);
        } else {
            printf("%d. Continue straight to %s and continue for %.2fm\n", step++, new_name, new_accumulated_length);
        }

        // Actualizamos las variables para el siguiente cambio de calle
        last_segment_of_current_street = current_segment;
        current_name = new_name;
    }

    printf("--- You have arrived to your destination ---\n");
}