#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // Required for strcasecmp
#include "find.h"
#include "streets_utils.h"
#include "levenshtein.h"
#include <float.h> 
#include <math.h>
#include "route.h"
#define EARTH_RADIUS 6371.0

void free_houses(House* head) {
   House* temp;
   while (head != NULL) {
       temp = head;
       head = head->next;
       free(temp);
   }
}

void free_place(Place* head) {
   Place* temp;
   while (head != NULL) {
       temp = head;
       head = head->next;
       free(temp);
   }
}

void free_streets(Street* head){
    Street* curr = head;
    while(curr != NULL){
        Street* next_node = curr->next;
        free(curr);
        curr = next_node;

    }
}

//we need to get into the file and store houses in a linked list
House* load_houses(const char* f){
    FILE* file = fopen(f, "r");//open file to read it
    if (file==NULL){
        printf("Couldn't open file %s.\n", f);
        return NULL;//if it couldn't be opened
    } 

    //Let's create a linked list
    House* head = NULL;
    char line[256];//to store each line of file (house)

    while (fgets(line, sizeof(line), file)) {
    House* newHouse = malloc(sizeof(House));
    if (!newHouse) break;

    int filled= sscanf(line, "%[^,],%d,%lf,%lf", newHouse->street_name, &newHouse->house_number, &newHouse->lat, &newHouse->lon);
    
    if (filled==4){//it was accessed and stored
        newHouse->next = head;//we link it to the list
        head = newHouse;
    } else {
        free(newHouse); // free memory if errors occured
    }
}
    fclose(file);
    return head; //it returns a pointer to the first house
}

//we need to get into the file and store places in a linked list
Place* load_places(const char* f){
    FILE *file = fopen(f,"r");
    if (file == NULL) {
        printf("Couldn't open file %s.\n", f);
        return NULL;
     } //if the file cannot be opened
    char line[256];

    Place *head=NULL;

    //Read the string and store its info in a new Place 
    while(fgets(line, sizeof(line), file)){
        Place *newp = malloc(sizeof(Place));
        if (!newp) break; //if malloc returned NULL

        int filled= sscanf(line, "%[^,],%[^,],%[^,],%lf,%lf", newp->id, newp->name, newp->category, &newp->latitude, &newp->longitude);

        if (filled==5){ //it successfully added all info to a new Place
            newp->next=head;
            head= newp;
        } else{
            free(newp);//if it couldn't be loaded free the space
        }
    }
    fclose(file);
    return head; //returns the pointer to the first position of the linked list of places.
}

//Find and print coordinates if it chose address (1)
Street* find_address(House* head, Street* all_streets) {
    char search_street[256];
    int search_number;

    printf("Enter street name (e.g. \"Carrer de Roc Boronat\"): ");
    fgets(search_street, sizeof(search_street), stdin);
    search_street[strcspn(search_street, "\n")] = 0;

    while (1) { 
        printf("Enter street number: ");
        if (scanf("%d", &search_number) != 1) {
            printf("Invalid number input.\n");
            return NULL;
        }
        while (getchar() != '\n');

        House* current = head;
        int street_found = 0; 
        
        while (current != NULL) {
            char norm_input[256];
            char norm_street[256]; 
            
            normalizeStreetName(norm_input, search_street);
            normalizeStreetName(norm_street, current->street_name);

            if (strcasecmp(norm_input, norm_street) == 0 && current->house_number == search_number) {
                printf("Found at (%f, %f)\n", current->lat, current->lon);
                
                
                Street* closest = get_closest_street(current->lat, current->lon, all_streets);
                
                return closest; // Devolvemos la calle encontrada al main
            }
            else if(strcasecmp(norm_input, norm_street) == 0) {
                street_found = 1;
            }
            current = current->next; 
        }
        
        int choice = 0;
        if (street_found == 1) {
            printf("The street name is correct, but the number is not valid.\n");
            printf("You have 2 options:\n1. Change number\n2. Change street\n");
            choice = option_menu(1, 2);
        }
        if (choice == 2 || street_found == 0) {
            strcpy(search_street, similar_streets(search_street, head));
        }
    }
    return NULL; 
}

//Prints the coordinates based on place
void find_place(Place* head) {
    char search_place[256];

    printf("Enter place name (e.g. Universitat Pompeu Fabra-Campus del Poblenou or L'Illa Diagonal): ");
    fgets(search_place, sizeof(search_place), stdin);
        // Remove the newline character from the string
        search_place[strcspn(search_place, "\n")] = 0;
    
    while (1) {
        //Sequential search
        Place *current= head;
        while(current!=NULL){
            if (strcasecmp(search_place,current->name)==0){
                printf("Found at (%f, %f)\n", current->latitude, current->longitude);
                return;
            }
            current= current->next;
        }
        
        //finished all places and didn't find it:
        strcpy(search_place, similar_places(search_place, head));
    }
}

// Quick helper to check if a number is prime
int is_prime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}

// Finds the next prime number larger than your estimate
int get_next_prime(int n) {
    if (n <= 1) return 2;
    int prime = n;
    while (!is_prime(prime)) {
        prime++;
    }
    return prime;
}

void load_streets(const char* f, Street** street_head, Node** nodes_head, int* GridSize) {
    FILE* file = fopen(f, "r");
    if (file == NULL) {
        printf("\n[SOS] load_streets: No he podido abrir el archivo: %s\n", f);
        return;
    }
    
    char line[256];
    Street *head = NULL;
    
    long long id1, id2; 
    double lat1, lon1, lat2, lon2;
    
    double max_lat = -90.0; 
    double min_lat = 90.0;
    double max_lon = -180.0; 
    double min_lon = 180.0;

    *nodes_head = NULL; 

    while (fgets(line, sizeof(line), file)) {
        Street* new_street = malloc(sizeof(Street));
        if (new_street == NULL) break;
        
        int filled = sscanf(line, "%lld, %lf, %lf, %lld, %lf, %lf, %lf, %[^\n]", 
                            &id1, &lat1, &lon1, &id2, &lat2, &lon2, &new_street->length, new_street->street_name);
        
        if (filled == 8) {
            if (lat1 > max_lat) max_lat = lat1;
            if (lat1 < min_lat) min_lat = lat1;
            if (lon1 > max_lon) max_lon = lon1;
            if (lon1 < min_lon) min_lon = lon1;
            if (lat2 > max_lat) max_lat = lat2;
            if (lat2 < min_lat) min_lat = lat2;
            if (lon2 > max_lon) max_lon = lon2;
            if (lon2 < min_lon) min_lon = lon2;

            new_street->start.id = id1;
            new_street->start.lat = lat1;
            new_street->start.lon = lon1;

            new_street->end.id = id2;
            new_street->end.lat = lat2;
            new_street->end.lon = lon2;

            new_street->mid_lat = (lat1 + lat2) / 2.0;
            new_street->mid_lon = (lon1 + lon2) / 2.0;
            
            new_street->next = head;
            head = new_street;
        } else {
            free(new_street);
        }
    }
    
    *street_head = head; 

    int rows = (max_lat - min_lat) / 0.01;
    int cols = (max_lon - min_lon) / 0.01;
    int estimated_boxes = rows * cols;

    if (estimated_boxes <= 0) estimated_boxes = 100; 
    if (is_prime(estimated_boxes) == 1) {
        *GridSize = estimated_boxes;
    } else {
        *GridSize = get_next_prime(estimated_boxes); 
    }

    fclose(file);
}

//Linear search through all the streets list to find connected streets --> share nodes
char** connected_streets(Street* head, const char* target_name){
    //find all streets connected to target
    Street* target_street=NULL;
    Street* current= head;
    int connections=0;
    int current_size=100;
    char** connected_to=malloc(current_size*sizeof(char*));
    if (connected_to==NULL){
        return NULL;
    }

    while (current!=NULL){// loop to find the street strc of the targeted street
        if (strcasecmp(target_name,current->street_name)==0){
            target_street=current;
            break;//we already found it
        }
        current=current->next;//keep iterating
    }

    if (target_street==NULL){
            return NULL;//handle if it doesn't find it
        }
    
    current= head;
    int node1=target_street->start.id;
    int node2=target_street->end.id;
    while (current!=NULL){ //loop to find connections
        if((current->start.id==node1||current->start.id==node2 || current->end.id==node1 || current->end.id==node2) && current!=target_street){
           if (connections>=current_size){  
            current_size*=2;
            char** temp=realloc(connected_to,current_size*sizeof(char*));
            if(temp!=NULL){
                connected_to=temp;
            }
           } 
            connected_to[connections]=current->street_name;
            connections++;
        }
        current=current->next;
    }

    if (connections<current_size){
        connected_to[connections]=NULL; //to indicate the caller where the array of pointers ends
    }

    return connected_to; //returns a list of the names of all streets connected to the target street
}

//CODE FOR THE NEW AND MORE EFFICIENT WAY TO FIND CLOSEST STREET
// CODE FOR SPATIAL HASHING:
void get_grid_index (double coord_x, double coord_y, int* grid_x, int* grid_y){
    *grid_x= coord_x/0.01;
    *grid_y= coord_y/0.01;
} //Function that transforms normal coordinates to grid coordinates

int hash_function(int row, int col, int grid_size) { //grid size will be given by load_streets
    unsigned int hash = (row * 73856093) ^ (col * 19349663);
    return hash % grid_size;
} //to get the index of the 1D array of GridBoxes

//Function that takes a street and allocates it into its respective GridBox
void street_to_box (Street* street,GridBox** GridBoxes, int grid_size){ //receives the head pointer of all grid boxes (the main grid)

    int grid_x, grid_y; 
    
    get_grid_index(street->mid_lon, street->mid_lat, &grid_x, &grid_y);
    
    int index = hash_function(grid_x, grid_y, grid_size);

    GridBox* current_box = GridBoxes[index];
    GridBox* target_box = NULL;

    while (current_box != NULL) {
        if (current_box->col == grid_x && current_box->row == grid_y) {
            target_box = current_box;
            break;
        }
        current_box = current_box->next;
    }

    //if gridBox with that row and col doesn't exist, create it.
    if (target_box == NULL) {
        target_box = malloc(sizeof(GridBox));
        if (target_box == NULL) return; 

        target_box->col = grid_x;
        target_box->row = grid_y;
        target_box->streets = NULL; 
        
        target_box->next = GridBoxes[index];
        GridBoxes[index] = target_box;
    }

    // insert the street into the target box street list
    StreetNode* new_StreetNode = malloc(sizeof(StreetNode));
    if (new_StreetNode != NULL) {
        new_StreetNode->street = street;
        
        //push to the head of the streets linked list for this box
        new_StreetNode->next = target_box->streets;
        target_box->streets = new_StreetNode;
    }
    
}

//CODE FOR INTERSECTION HASH MAPPING
int intersection_hash(long long target_id, int graph_size) {
    int index = target_id % graph_size;
    if (index < 0) {
        index += graph_size;
    }
    return index;
}

void street_to_intersection(IntersectionBucket** graph, int graph_size, long long target_id, Street* street) {
    
    
    int index = intersection_hash(target_id, graph_size);

    IntersectionBucket* current = graph[index];

    IntersectionBucket* targetBucket = NULL;

    while (current != NULL) {
        if (current->intersection_id == target_id) {
            targetBucket = current;
            break;
        }
        current = current->next;
    }

    if (targetBucket == NULL) {
        targetBucket = malloc(sizeof(IntersectionBucket));
        if (targetBucket == NULL) return;
        
        targetBucket->intersection_id = target_id;
        targetBucket->connected_streets = NULL;
        
        targetBucket->next = graph[index];
        graph[index] = targetBucket;
    }
    
    StreetNode* new_node = malloc(sizeof(StreetNode));
    if (new_node != NULL) {
        new_node->street = street;
        new_node->next = targetBucket->connected_streets;
        targetBucket->connected_streets = new_node;
    }
}

double haversine(Position posA, Position posB) {
    double lat1 = toRadians(posA.lat);
    double lon1 = toRadians(posA.lon);
    double lat2 = toRadians(posB.lat);
    double lon2 = toRadians(posB.lon);

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    double a = pow(sin(dLat / 2), 2) +
               cos(lat1) * cos(lat2) * pow(sin(dLon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS * c;
}


Street* get_closest_street(double user_lat, double user_lon, Street* all_streets) {
    Street* current = all_streets;
    Street* closest_street = NULL;
    double min_distance = DBL_MAX;

    Position target_pos = {user_lat, user_lon};

    while (current != NULL) {
        Position mid_pos = {current->mid_lat, current->mid_lon};

        double distance = haversine(target_pos, mid_pos);

        if (distance < min_distance) {
            min_distance = distance;
            closest_street = current;
        }

        current = current->next;
    }

    return closest_street; 
}




