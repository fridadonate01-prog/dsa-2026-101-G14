#include "streets_utils.h"
#include <string.h>

void normalizeStreetName(char* destination, const char* original) {
    if (strncmp(original, "C. ", 3) == 0 || strncmp(original, "C/ ", 3) == 0) {
        strcpy(destination, "Carrer ");
        strcat(destination, original + 3);
    } else {
        strcpy(destination, original);
    }
}