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
    
    if (filled==4){//it was access and stored
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

void load_streets(const char* f, Street** street_head, Node** nodes_head){
    FILE* file= fopen(f, "r");
    if (file==NULL){
        return;
    }
    char line[256];
    Street *head= NULL;
    double lat1, lon1, lat2, lon2;

    int current_capacity= 1000;
    Node* nodes=malloc(current_capacity*sizeof(Node));
    
    while (fgets(line, sizeof(line),file)){
      Street* new_street= malloc(sizeof(Street));
      if (new_street==NULL) break;
      int filled = sscanf(line, "%d, %lf, %lf, %d, %lf, %lf, %d, %[^,]", &new_street->start_id, &lat1,&lon1,&new_street->end_id, &lat2, &lon2,&new_street->length, new_street->street_name);
      if (filled==8){
        int highest_id_line;
        if (new_street->start_id>=new_street->end_id){
            highest_id_line= new_street->start_id;
        } else{
            highest_id_line =new_street->end_id;
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
        nodes[new_street->start_id].lat=lat1;//storing the two nodes of line into the list of nodes
        nodes[new_street->start_id].lon=lon1;
        nodes[new_street->end_id].lat=lat2;
        nodes[new_street->end_id].lon=lon2;
        new_street->next= head;
        head = new_street;
      } else {
        free(new_street);
      }
    }
    *street_head= head;//saving the addresses in global pointer so they are not lost when function ends.
    *nodes_head= nodes;// we use * to dereference the double pointers of the main

    fclose(file);
}