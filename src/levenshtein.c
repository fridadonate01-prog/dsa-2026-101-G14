#include "levenshtein.h"
#include "find.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
int levenshtein_distance(char *input_str, char *database_str){
    int inp_len = strlen(input_str);
    int dat_len = strlen(database_str);

    int matrix[inp_len+1][dat_len+1];


    for (int i = 0; i < inp_len + 1; i++){
        matrix[i][0] = i;
    }
    for (int j = 0; j < dat_len + 1; j++){
        matrix[0][j] = j;
    }
    int cost;
    for (int i = 1; i < inp_len + 1; i++){
        for (int j = 1; j < dat_len + 1; j++){

            if (tolower(input_str[i-1])==tolower(database_str[j-1])){
                cost = 0; 
            }
            else{
                cost = 1;
            }
            matrix[i][j] = min_three(matrix[i-1][j]+1, // deletion
                matrix[i][j-1] + 1, // insertion
                matrix[i-1][j-1] + cost // substitution
            );

        }
    }
    return matrix[inp_len][dat_len];


}

int min_three (int a, int b, int c){
    if (a <= b && a <= c){return a;}
    else if (b <= a && b <= c){return b;}
    return c;
}

char * similar_streets(char *inp_string, House *head){
    int num_options = 5;
    char current_search[256];
    strcpy(current_search, inp_string);
    while (1){
        Options similar_options[num_options];
        for (int i = 0; i < num_options; i++){
            similar_options[i].distance = 10000; // high number to initialise the value
            strcpy(similar_options[i].name, "...................");
        }

        House * current = head;
        while(current != NULL){
            int is_clon = 0; // for repeated streets in the data, i.e. Roc Boronat 138 and Roc Boronat 140
            for (int k = 0; k < num_options; k++){
                if (strcmp(similar_options[k].name, current->street_name) == 0){
                    is_clon = 1;
                    break;
                }
            }
            if (is_clon == 0){
                int current_levenshtein_dist = levenshtein_distance(current_search, current->street_name);
                if (current_levenshtein_dist == 0) { //we found a perfect match
                    printf("the street you wrote has an exact match! (%s).\n",current->street_name );
                    return strdup(current->street_name); //return this directly
                }
                if (current_levenshtein_dist < similar_options[num_options-1].distance){ // the distance is lower than the last option (lowest match)
                    for (int i = 0; i < num_options; i++){
                        if (current_levenshtein_dist < similar_options[i].distance){ // we check in order to make an order of match
                            // strcpy(similar_options[i].name, current->street_name); // we update the options list with the new street
                            // similar_options[i].distance = current_levenshtein_dist; // ... and it's new distance
                            for (int j = num_options-1; j > i; j--){ //for all the worse options that come after
                                similar_options[j].distance = similar_options[j-1].distance;
                                strcpy(similar_options[j].name, similar_options[j-1].name);
                            }
                            strcpy(similar_options[i].name, current->street_name); // we update the options list with the new street
                            similar_options[i].distance = current_levenshtein_dist; // ... and it's new distance
                            break;


                        }
                    }
                }
            }
            current = current->next;

        }
        printf("\nThat street name is not known! Did you mean...\n");
        for (int i = 0; i < num_options; i++){ 
            printf("  %d. %s\n", i+1, similar_options[i].name);
        }
        printf("  0. !! None of these, let me type again !!\n");

        int choice = option_menu(0,num_options);

        if (choice >= 1 && choice <= num_options) {
        
            return strdup(similar_options[choice-1].name); 

        } else if (choice == 0) {
            printf("Enter the new street name: ");
            fgets(current_search, sizeof(current_search), stdin);
            current_search[strcspn(current_search, "\n")] = 0; // Quitar el '\n' maldito

        } 
    }

}

int option_menu(int low_lim, int high_lim){
    while (1){
            int choice;
            printf("\nChoose an option (%d-%d): ",low_lim, high_lim);
            
            if (scanf("%d", &choice) != 1) {
                while (getchar() != '\n'); // Limpiar buffer si mete letras
                printf("Invalid input. Please enter a number.\n");
                continue; // Reinicia el bucle (vuelve a calcular, pero no pasa nada)
            }
            while (getchar() != '\n'); // Limpiar el salto de línea del scanf

            if (choice >= low_lim && choice <= high_lim) {
            
                return choice; 

            } 
            else {
                printf("Invalid choice. Try again.\n");
        }
    }
        
}


char * similar_places(char *inp_string, Place *head){
    int num_options = 5;
    char current_search[256];
    strcpy(current_search, inp_string);
    
    while (1){
        Options similar_options[num_options];
        for (int i = 0; i < num_options; i++){
            similar_options[i].distance = 10000; 
            strcpy(similar_options[i].name, "...................");
        }

        Place * current = head;
        while(current != NULL){
            int is_clon = 0; 
            for (int k = 0; k < num_options; k++){
                if (strcasecmp(similar_options[k].name, current->name) == 0){
                    is_clon = 1;
                    break;
                }
            }
            if (is_clon == 0){
                int current_levenshtein_dist = levenshtein_distance(current_search, current->name);
                
                if (current_levenshtein_dist == 0) {
                    return strdup(current->name);
                }

                if (current_levenshtein_dist < similar_options[num_options-1].distance){ 
                    for (int i = 0; i < num_options; i++){
                        if (current_levenshtein_dist < similar_options[i].distance){ 
                            for (int j = num_options-1; j > i; j--){ 
                                similar_options[j].distance = similar_options[j-1].distance;
                                strcpy(similar_options[j].name, similar_options[j-1].name);
                            }
                            strcpy(similar_options[i].name, current->name); 
                            similar_options[i].distance = current_levenshtein_dist; 
                            break;
                        }
                    }
                }
            }
            current = current->next;
        }

        printf("\nThat place name is not known! Did you mean...\n");
        for (int i = 0; i < num_options; i++){ 
            printf("  %d. %s\n", i+1, similar_options[i].name);
        }
        printf("  0. !! None of these, let me type again !!\n");

        int choice;
        printf("\nChoose an option (0-%d): ", num_options);
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            printf("Invalid input. Please enter a number.\n");
            continue; 
        }
        while (getchar() != '\n'); 

        if (choice >= 1 && choice <= num_options) {
            return strdup(similar_options[choice-1].name); 
        } else if (choice == 0) {
            printf("Enter the new place name: ");
            fgets(current_search, sizeof(current_search), stdin);
            current_search[strcspn(current_search, "\n")] = 0; 
        } else {
            printf("Invalid choice. Try again.\n");
        }
    }
}