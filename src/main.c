#include "sample_lib.h"
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
  const char * all_location_inputs[] = {"address", "place", "coordinate"};
  // Call the function and store the result
  const char * chosen_map = choose_map(all_maps); 
  const char * chosen_input = choose_input (all_location_inputs);

  printf("the chosen map is: %s\n",chosen_map);
  printf("the chosen input is: %s\n",chosen_input);

  if (strcmp(chosen_input,"coordinate") == 0 || strcmp(chosen_input,"address") == 0){printf("Not implemented yet.\n");  }

  char file_path[100];

// Construct path: assuming execution from 'src' folder
    sprintf(file_path, "../maps/%s/houses.txt", chosen_map);

    House* my_houses = load_houses(file_path);

    if (my_houses != NULL) {
        find_address_logic(my_houses);
    } else {
        printf("Error: Could not load map data.\n");
    }

    return 0;
}