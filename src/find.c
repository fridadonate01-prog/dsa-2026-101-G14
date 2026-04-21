#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // Required for strcasecmp
#include "find.h"
#include <ctype.h>

//we need to get into the file and store houses in a linked list
House* load_houses(const char* f){
    FILE* file = fopen(f, "r");//open file to read it
    if (file==NULL) return NULL;//if it cannot open it return null

    //Let's create a linked list
    House* head = NULL;
    char line[256];//to store each line of file (house)
    int houses=0; //counts houses

    while(fgets(line, 256,file)){//while there's a line...
        House* newHouse= malloc(sizeof(House));
        newHouse->lat=0;
        newHouse->lon=0;
        int info_count=0;//how many information we have separated from line
        int j = 0; // Local index for the street_name array


        for (int i=0; line[i]!='\0'&& line[i]!='\n'; i++){
            char current= line[i];

            if (line[i]==','){
                info_count++;
                j=0; //reset index
                continue; //skip the comma
            }

            switch(info_count){
                case 0:
                    newHouse ->street_name[j++]=current;
                    newHouse->street_name[j]= '\n';
                    break;
                case 1://house number
                    sscanf(&line[i], "%d", &newHouse->house_number);
                    while (isdigit(line[i+1])) i++; // Fast-forward
                    break;
                case 2: //we need lat
                    sscanf(&line[i], "%lf", &newHouse->lat);
                    while (isdigit(line[i+1]) || line[i+1] == '.') i++;
                    break;
                case 3:
                    sscanf(&line[i], "%lf", &newHouse->lon);
                    while (isdigit(line[i+1]) || line[i+1] == '.' || line[i+1] == '-') i++;
                    break;
            }
        }
    newHouse->next = head;
    head= newHouse;
    }
    fclose(file);
    return head; //it returns a pointer to the first house
}


//we need to get into the file and store places in a linked list
Place* load_places(const char* f){
    FILE *file = fopen(f,"r");
    if (file == NULL) return NULL; //if the file cannot be opened
    char line[256];

    Place *head=NULL;

    //Read the string and store its info in a new Place 
    while(fgets(line, sizeof(line), file)){
        Place *newp = malloc(sizeof(Place));
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
    char search_street[100];
    int search_number;

    printf("Enter street name (e.g. \"Carrer de Roc Boronat\"): ");

    fgets(search_street, sizeof(search_street), stdin);
        // Remove the newline character from the string
        search_street[strcspn(search_street, "\n")] = 0;


    printf("Enter street number: ");
    if (scanf("%d", &search_number) != 1) {
        printf("Invalid number input.\n");
        return;
        }
    // Clear the buffer so the next fgets works correctly
    while (getchar() != '\n');

    //Sequential search to find the house
    House* current = head;//current house pointer
    while (current != NULL) { 
        if (strcasecmp(search_street,current->street_name)==0 && current->house_number==search_number){
            printf("Found at (%f, %f)\n", current->lat, current->lon);
            return;
        }
        current= current->next; //not found, so look for the next

    }

    //finished all houses and didn't find it:
    printf("Address not found. \n"); 
}//it prints the coordinates based on address

void find_place(Place* head) {
    char search_place[100];

    printf("Enter place name (e.g. Universitat Pompeu Fabra-Campus del Poblenou or L'Illa Diagonal): ");
    fgets(search_place, sizeof(search_place), stdin);
        // Remove the newline character from the string
        search_place[strcspn(search_place, "\n")] = 0;
    
    //Sequential search
    Place *current= head;
    while(current!=NULL){
        if (strcasecmp(search_place,current->name)==0){
            printf("Found at (%f, %f)", current->latitude, current->longitude);
            return;
        }
        current= current->next;
    }
    //finished all places and didn't find it:
    printf("Place not found. \n"); 

}//it prints the coordinates based on place
