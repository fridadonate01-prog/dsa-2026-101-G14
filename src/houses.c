#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // Required for strcasecmp
#include "houses.h"

// --- IMAGE IMPLEMENTATION: READING, PARSING, AND LINKED LIST ---
House* load_houses(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;

    House* head = NULL;
    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        House* new_house = malloc(sizeof(House));
        if (sscanf(line, "%[^,],%d,%lf,%lf", new_house->street_name, 
                   &new_house->house_number, &new_house->lat, &new_house->lon) == 4) {
            new_house->next = head;
            head = new_house;
            count++;
        } else {
            free(new_house);
        }
    }
    printf("%d houses loaded\n", count); // Matches example output
    fclose(file);
    return head;
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