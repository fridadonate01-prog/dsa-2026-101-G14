#include "input.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "find.h"
#include "route.h"

int main() {
  // Array of string pointers
  const char * all_maps[] = {"xs_1", "xs_2", "md_1", "lg_1", "xl_1", "2xl_1"}; 
  
  // Call the function and store the result
  int chosen_map = choose_map(all_maps); 

  // We need to load the map's streets so the BFS algorithm can use them
  char street_file_path[100];
  sprintf(street_file_path, "maps/%s/streets.txt", all_maps[chosen_map]);
  Street* all_streets = load_streets(street_file_path);

  // Variables to hold the starting and ending streets
  Street* origin_street = NULL;
  Street* dest_street = NULL;

  // ORIGIN SECTION
  int chosen_input = choose_input (); //input has the choice number
  char file_path[100];
  if (chosen_input==1){//the user chose address
    // Construct path: assuming execution from 'src' folder
    sprintf(file_path, "maps/%s/houses.txt", all_maps[chosen_map]);//file path contains the name of the file path to the houses

    House* my_houses = load_houses(file_path); //pointer to the first house

    find_address(my_houses);
    free_houses(my_houses);
  } else if(chosen_input==2){//the user chose place
    //Construct path: assuming execution form 'src' folder
    sprintf(file_path,"maps/%s/places.txt", all_maps[chosen_map]);//file path contains the name of the file path to the places

    Place* my_places= load_places(file_path); //pointer to the first place

    find_place(my_places);
    free_place(my_places);
  } else{
    printf("Not implemented yet. \n");
  }

  //DESTINATION SECTION
  int chosen_dest = choosen_destination();

  if (chosen_dest == 1) { // the user chose address
    sprintf(file_path, "maps/%s/houses.txt", all_maps[chosen_map]);
    House* dest_houses = load_houses(file_path);
    find_address(dest_houses);
    free_houses(dest_houses);
  }
  else if (chosen_dest == 2) {  // the user chose place
    sprintf(file_path, "maps/%s/places.txt", all_maps[chosen_map]);
    Place* dest_places = load_places(file_path);
    find_place(dest_places);
    free_place(dest_places);
  }
  else {
    printf("Not implemented yet. \n");
  }

  // GENERATE AND PRINT THE ROUTE
  // If both an origin and destination were succesfully found, run the BFS
  if (origin_street != NULL && dest_street != NULL) {
    PathNode* final_route = find_route(origin_street, dest_street, all_streets);

    print_route_directions(final_route);

    free_path(final_route);
  }
  
  free_streets(all_streets);

  return 0;
}