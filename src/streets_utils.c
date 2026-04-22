#include "streets_utils.h"
#include <string.h>

void normalizeStreetName(char* destination, const char* original) {
    if (strncmp(original, "C. ", 3) == 0 || strncmp(original, "C/ ", 3) == 0) {
        strcpy(destination, "Carrer ");
        strcat(destination, original + 3);
    }else if(strncmp(original, "Av. ", 4) == 0 || strncmp(original, "Av/ ", 4) == 0){
        strcpy(destination, "Avinguda ");
        strcat(destination, original + 4);
    }else if(strncmp(original, "Pass. ", 6) == 0 || strncmp(original, "Pass/ ", 6) == 0){
        strcpy(destination, "Passeig ");
        strcat(destination, original + 6);
    }else {
        strcpy(destination, original);
    }
}


// Avinguda (Av. o Av/), Passeig(Pass. o Pass/)