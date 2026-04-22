#include "input.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "find.h"

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
  int chosen_input = choose_input (); //input has the choice number

  char file_path[100];
  if (chosen_input==1){//the user chose address
    // Construct path: assuming execution from 'src' folder
    sprintf(file_path, "maps/%s/houses.txt", all_maps[chosen_map]);//file path contains the name of the file path to the houses

    House* my_houses = load_houses(file_path); //pointer to the first house

    find_address(my_houses);
  } else if(chosen_input==2){//the user chose place
    //Construct path: assuming execution form 'src' folder
    sprintf(file_path,"maps/%s/places.txt", all_maps[chosen_map]);//file path contains the name of the file path to the places

    Place* my_places= load_places(file_path); //pointer to the first place

    find_place(my_places);
  } else{
    printf("Not implemented yet. \n");
  }

    return 0;
}