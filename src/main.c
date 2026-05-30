#include "input.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include "find.h"
#include "route.h"

int main() {
  // Array of string pointers
  const char * all_maps[] = {"xs_1", "xs_2", "md_1", "lg_1", "xl_1", "2xl_1"}; 
  
  // Call the function and store the result
  int chosen_map = choose_map(all_maps); 

  // We need to load the map's streets so the BFS algorithm can use them
  char street_file_path[100];
  sprintf(street_file_path, "../maps/%s/streets.txt", all_maps[chosen_map]);

  // Set up the 4 required parameters to call loading functions
  Street* all_streets = NULL;
  Node* all_nodes = NULL;
  int grid_size = 0;
  load_streets(street_file_path, &all_streets, &all_nodes, &grid_size);

  // Variables to hold the starting and ending streets
  Street* origin_street = NULL;
  Street* dest_street = NULL;

  // ORIGIN SECTION
  int chosen_input = choose_input (); //input has the choice number
  char file_path[100];
  if (chosen_input==1){//the user chose address
    // Construct path: assuming execution from 'src' folder
    sprintf(file_path, "../maps/%s/houses.txt", all_maps[chosen_map]);//file path contains the name of the file path to the houses

    House* my_houses = load_houses(file_path); //pointer to the first house

    origin_street = find_address(my_houses, all_streets);
    free_houses(my_houses);
  } else if(chosen_input==2){//the user chose place
    //Construct path: assuming execution form 'src' folder
    sprintf(file_path,"../maps/%s/places.txt", all_maps[chosen_map]);//file path contains the name of the file path to the places

    Place* my_places= load_places(file_path); //pointer to the first place

    origin_street = find_place(my_places, all_streets);
    free_place(my_places);
  } else if (chosen_input == 3) { // User chose Coordinates
    double user_lat, user_lon;
    char coordinate_buffer[100];

    printf("Enter Origin Latitude and Longitude (separated by a space, e.g., 41.3851 2.1734):\n");
    if (fgets(coordinate_buffer, sizeof(coordinate_buffer), stdin) != NULL) {
      if (sscanf(coordinate_buffer, "%lf %lf", &user_lat, &user_lon) == 2) {
        // Find the closest street using your Haversine-based lookup function
        origin_street = get_closest_street(user_lat, user_lon, all_streets);
        if (origin_street != NULL) {
          printf("Origin locked to nearest street: %s\n", origin_street->street_name);
        } else {
          printf("Error: No streets found on the map matching those coordinates. \n");
        }
      } else {
        printf("Error: Invalid coordinate input format. \n");
      }
    }
  }

  //DESTINATION SECTION
  int chosen_dest = choose_destination();

  if (chosen_dest == 1) { // the user chose address
    sprintf(file_path, "../maps/%s/houses.txt", all_maps[chosen_map]);
    House* dest_houses = load_houses(file_path);
    dest_street = find_address(dest_houses, all_streets);
    free_houses(dest_houses);
  }
  else if (chosen_dest == 2) {  // the user chose place
    sprintf(file_path, "../maps/%s/places.txt", all_maps[chosen_map]);
    Place* dest_places = load_places(file_path);
    dest_street=find_place(dest_places,all_streets);
    free_place(dest_places);
  } else if (chosen_dest == 3) { // User chose Coordinates
    double user_lat, user_lon;
    char coordinate_buffer[100];  
    
    printf("Enter Destination Latitude and Longitude (separated by a space, e.g. 41.3982 2.1421:\n)");
    if (fgets(coordinate_buffer, sizeof(coordinate_buffer), stdin) != NULL) {
      if (sscanf(coordinate_buffer, "%lf %lf", &user_lat, &user_lon) == 2) {
        // Find the closest destination street
        dest_street = get_closest_street(user_lat, user_lon, all_streets);
        if (dest_street != NULL) {
          printf("Destination locked to nearest street: %s\n", dest_street->street_name);
        } else {
          printf("Error: No streets found on the map matching those coordinates. \n");
        }
      } else {
        printf("Error: Invalid coordinate input format. \n");
      }
    }
  }
  if (all_streets == NULL) {
      printf("\n[CRITICAL ERROR]: all_streets is empty (NULL). Check your load_streets function or the file path to streets.txt.\n");
      return 1; // Salimos antes de explotar
  }

  if (origin_street == NULL) {
      printf("\n[ERROR]: Could not calculate the closest origin street.\n");
  } else {
      printf("TEST, here origin street is worth: %s\n", origin_street->street_name);
  }

  if (dest_street == NULL) {
      printf("\n[ERROR]: Could not calculate the closest destination street.\n");
  } else {
      printf("TEST, here dest street is worth: %s\n", dest_street->street_name);
  }


// Initialize and fill the Intersection Hash Map Graph before routing
  IntersectionBucket** graph = calloc(grid_size, sizeof(IntersectionBucket*));
  if (graph != NULL) {
    Street* curr_st = all_streets;
    while (curr_st != NULL) {
      // Connect each street to BOTH of its intersection nodes
      street_to_intersection(graph, grid_size, curr_st->start.id, curr_st);
      street_to_intersection(graph, grid_size, curr_st->end.id, curr_st);
      curr_st = curr_st->next;
    }
  }

  // GENERATE AND PRINT THE ROUTE
  // If both an origin and destination were succesfully found, run the BFS
  if (origin_street != NULL && dest_street != NULL) {
    
    // Start Timer
    clock_t start = clock();

    StreetNode* final_route = find_route(origin_street, dest_street, all_streets, graph, grid_size);

    // Stop Timer
    clock_t end = clock();

    // Calculate latency in miliseconds
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
    printf("\nLatency Results for map '%s' :\n", all_maps[chosen_map]);
    printf("Time taken: %f ms\n", time_spent);

    print_route_directions(final_route);

    free_path(final_route);
  }
  
  // Clean up all dynamically allocated graph memory safely
  free_streets(all_streets);
  if (all_nodes != NULL) {
    free(all_nodes);
  }
  if (graph != NULL) {
    for (int i = 0; i < grid_size; i++) {
      IntersectionBucket* curr_bucket = graph[i];
      while(curr_bucket != NULL) {
        IntersectionBucket* temp_bucket = curr_bucket;
        curr_bucket = curr_bucket->next;

       // Free the internal linked list node wrappers
        StreetNode* curr_sn = temp_bucket->connected_streets;
        while (curr_sn != NULL) {
          StreetNode* temp_sn = curr_sn;
          curr_sn = curr_sn->next;
          free(temp_sn);
        }
        free(temp_bucket);
      }
    }
    free(graph);
  }

  return 0;
}