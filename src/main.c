#include "input.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "houses.h"
#include <string.h>

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {
  // Array of string pointers
  const char * all_maps[] = {"xs_1", "xs_2", "md_1", "lg_1", "xl_1", "2xl_1"}; 
  // const char * all_location_inputs[] = {"empty to make nums match","address", "place", "coordinate"};
  
  // Call the function and store the result
  int chosen_map = choose_map(all_maps); 
  int chosen_input = choose_input ();



  if ( chosen_input == 2 || chosen_input == 3){printf("Not implemented yet.\n");  }

  char file_path[100];

// Construct path: assuming execution from 'src' folder
    sprintf(file_path, "maps/%s/houses.txt", all_maps[chosen_map]);

    House* my_houses = load_houses(file_path);

    if (my_houses != NULL) {
        find_address_logic(my_houses, chosen_input);
    } else {
        printf("Error: Could not load map data.\n");
    }

    return 0;
}