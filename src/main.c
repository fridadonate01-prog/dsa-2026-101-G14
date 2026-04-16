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
  // Array of string pointers
  const char * maps[] = {"xs_1", "xs_2", "md_1", "lg_1", "xl_1", "2xl_1"}; 
  
  // Call the function and store the result
  const char * map_name = choose_map(maps); 
  char file_path[100];

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