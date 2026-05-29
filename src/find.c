#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // Required for strcasecmp
#include "find.h"
#include <ctype.h>
#include "streets_utils.h"
#include "levenshtein.h"

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
void find_address (House* head){
    char search_street[256];
    int search_number;

    printf("Enter street name (e.g. \"Carrer de Roc Boronat\"): ");

    fgets(search_street, sizeof(search_street), stdin);
        // Remove the newline character from the string
        search_street[strcspn(search_street, "\n")] = 0;

    while (1){ //until we find a match
        printf("Enter street number: ");
        if (scanf("%d", &search_number) != 1) {
            printf("Invalid number input.\n");
            return;
            }
        // Clear the buffer so the next fgets works correctly
        while (getchar() != '\n');

        //Sequential search to find the house
        House* current = head;//current house pointer
        int street_found = 0; // to check if we find the street, but not the number
        while (current != NULL) {
            // 1. Declare buffers to store normalized names
            char norm_input[256];
            char norm_street[256]; 
            
            // 2. Normalize both the user input and the list street
            normalizeStreetName(norm_input, search_street);
            normalizeStreetName(norm_street, current->street_name);

            if (strcasecmp(norm_input, norm_street)==0 && current->house_number==search_number){
                printf("Found at (%f, %f)\n", current->lat, current->lon);
                return;
            }
            else if(strcasecmp(norm_input,norm_street)==0){ // same street, not same number!
                street_found = 1;
            }
            current= current->next; //not found, so look for the next
        }
        int choice = 0;
        if (street_found == 1){//but not number
            printf("the street name is correct, but the number is not valid.\n");
            printf("You have 2 options:\n");
            printf("1. Change the number to check if the address exists.\n");
            printf("2. Change the street name, hoping it holds your correct number address.\n");
            choice = option_menu(1, 2);
        }
        if (choice == 2 || street_found == 0) {
            strcpy(search_street, similar_streets(search_street,head));
        }
    }
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

void load_streets(const char* f, Street** street_head, Node** nodes_head, int* GridSize){
    FILE* file= fopen(f, "r");
    if (file==NULL){
        return;
    }
    char line[256];
    Street *head= NULL;
    double lat1, lon1, lat2, lon2;
    int id1, id2; 
    int max_lat=-90.0; 
    int min_lat=90.0;
    int max_lon=-180.0; 
    int min_lon=180.0;

    int current_capacity= 1000;
    Node* nodes=malloc(current_capacity*sizeof(Node));
    
    while (fgets(line, sizeof(line),file)){
      Street* new_street= malloc(sizeof(Street));
      if (new_street==NULL) break;
      int filled = sscanf(line, "%d, %lf, %lf, %d, %lf, %lf, %d, %[^\n]", &id1, &lat1,&lon1,&id2, &lat2, &lon2,&new_street->length, new_street->street_name);
      if (filled==8){
        int highest_id_line; //useful for allocating memory
        if (id1>=id2){
            highest_id_line= id1;
        } else{
            highest_id_line =id2;
        }
        if (highest_id_line>=current_capacity){
            current_capacity= highest_id_line+500; //buffer space
            Node* temp = realloc(nodes,current_capacity*sizeof(Node));//reallocate space so it fits into a new list in case it returns NULL
            if (temp==NULL){
                free(new_street);
                break;
            }
            nodes=temp;
            }
        //getting max and min lat and lon. useful for defining grid size for spatial hashing
        if (max_lat<lat1) max_lat=lat1;
        if (min_lat>lat1) min_lat=lat1;

        if (max_lon<lon1) max_lon=lon1;
        if (min_lon>lon1) min_lon=lon1;

        if (max_lat<lat2) max_lat=lat2;
        if (min_lat>lat2) min_lat=lat2;

        if (max_lon<lon2) max_lon=lon2;
        if (min_lon>lon2) min_lon=lon2;

        //storing the two nodes of line into the list of nodes
        nodes[id1].lat=lat1;
        nodes[id1].lon=lon1;
        nodes[id1].id=id1;

        nodes[id2].lat=lat2;
        nodes[id2].lon=lon2;
        nodes[id2].id=id2;

        new_street->next= head;
        head = new_street;
      } else {
        free(new_street);
      }
    }
    *street_head= head;//saving the addresses in global pointer so they are not lost when function ends.
    *nodes_head= nodes;// we use * to dereference the double pointers of the main

    //to get grid size (row and cols)
    int rows= (max_lat-min_lat)/0.01;
    int cols= (max_lon-min_lon)/0.01;
    int estimated_boxes = rows*cols;

    if (is_prime(estimated_boxes)==1){
        *GridSize= estimated_boxes;
    }else{
        *GridSize=get_next_prime(estimated_boxes);//we want a prime to get less collisions
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

    int grid_x1, grid_y1; //CHANGE! do for midpoint instead
    get_grid_index(street->start.lon,street->start.lat,&grid_x1,&grid_y1);

    int grid_x2, grid_y2;
    get_grid_index(street->end.lon,street->end.lat,&grid_x2,&grid_y2);

    int index1=hash_function(grid_x1,grid_y1,grid_size);
    int index2=hash_function(grid_x2,grid_y2,grid_size);

    GridBox* current_box = GridBoxes[index1];
    GridBox* target_box = NULL;

    while (current_box != NULL) {//find target box
        if (current_box->col == grid_x1 && current_box->row == grid_y1) {
            target_box = current_box;
            break;
        }
        current_box = current_box->next;
    }

    //if gridBox with that row and col doesn't exist, create it.
    if (target_box == NULL) {
        target_box = malloc(sizeof(GridBox));
        if (target_box == NULL) return; //malloc check

        target_box->col = grid_x1;
        target_box->row = grid_y1;
        target_box->streets = NULL; 
        
        // Insert at the head of the hash array linked list
        target_box->next = GridBoxes[index1];
        GridBoxes[index1] = target_box;
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
int intersection_hash(int intersection_id, int graph_size){
    return intersection_id%graph_size; //by modulo to get index
}

void street_to_intersection (IntersectionBucket** graph, int graph_size, int target_id, Street* street){
    //you have target id as a parameter bc each street has two nodes, so you have to indicate which one it is now
    int index= intersection_hash(target_id, graph_size);//where it will be mapped to

    IntersectionBucket* current= graph[index];
    IntersectionBucket* targetBucket= NULL;

    //get target bucket
    //1. is it already in map?
    while (current!=NULL){
        if (current->intersection_id==target_id){
            targetBucket=current;
            break;
        }
        current=current->next;
    }

    if (targetBucket==NULL){//2. it isn't yet, so create it
        //1) memory
        targetBucket= malloc(sizeof(IntersectionBucket));
        if (targetBucket==NULL) return;
        //2) fill
        targetBucket->intersection_id= target_id;
        targetBucket->connected_streets= NULL;
        //3) chain
        targetBucket->next= graph[index];
        graph[index]= targetBucket;
    }
    
    // Create a new node for the street list inside this bucket
    StreetNode* new_node = malloc(sizeof(StreetNode));
    if (new_node != NULL) {
        new_node->street = street;
        
    // Push to the head of the connected_streets linked list
    new_node->next = targetBucket->connected_streets;
    targetBucket->connected_streets = new_node;
    }

}
