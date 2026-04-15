#include "sample_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "houses.h"

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {
    char map_name[50];
    char file_path[100];

    // Ask for map name
    printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
    scanf("%s", map_name);

    // Construct path: assuming execution from 'src' folder
    sprintf(file_path, "../maps/%s/houses.txt", map_name);

    House* my_houses = load_houses(file_path);

    if (my_houses != NULL) {
        find_address_logic(my_houses);
    } else {
        printf("Error: Could not load map data.\n");
    }

    return 0;
}