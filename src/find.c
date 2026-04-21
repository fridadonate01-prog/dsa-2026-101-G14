#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // Required for strcasecmp
#include "find.h"

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


// --- POINT 4: FIND ADDRESS LOGIC ---
void find_address_logic(House* head, int choice) {
    char search_street[100];
    int search_number;

    // Matching the example prompts exactly
    if (choice == 1) {
        printf("Enter street name (e.g. \"Carrer de Roc Boronat\"): ");

        fgets(search_street, sizeof(search_street), stdin);
        search_street[strcspn(search_street, "\n")] = 0; // Remove newline

        printf("Enter street number (e.g. \"138\"): ");
        scanf("%d", &search_number);

        House* current = head;
        while (current != NULL) {
            // Case-insensitive comparison

            if (strcasecmp(current->street_name, search_street) == 0 && 
                current->house_number == search_number) {
                // Indented output to match the example
                printf("\n    Found at (%f, %f)\n", current->lat, current->lon);
                return;
            }
            current = current->next;
        }
        printf("\n    Address not found.\n");
    } else {
        printf("Not implemented yet.\n");
    }
}