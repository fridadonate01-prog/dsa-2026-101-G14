#include <stdio.h>
#include <string.h>

// 1. The function MUST be declared or defined BEFORE main()
// 2. The parameter must be an array of strings (const char *maps[])
const char* choose_map(const char *maps[]) {
  char chosen_map[50];

  while (1) { 
    printf("Enter map name (e.g. 'xs_2' or 'xl_1'): \n"); 
    scanf("%49s", chosen_map);
    while (getchar() != '\n');
    
    for (int i = 0; i < 6; i++) {
      
      if (strcmp(maps[i], chosen_map) == 0) {
          return maps[i]; // Return the pointer to the matched string
      } 
    }
    printf("Your input (%s) does not match with the possible options! Try again!\n", chosen_map);
  }
}

const char* choose_input(const char * all_location_inputs[]){
  printf("--- ORIGIN ---\n");
  int chosen_input; 
  char buffer[50];
  while (1) { 
    printf("Where are you? Address (1), Place (2) or Coordinate (3)?\n"); 
    fgets(buffer,sizeof(buffer),stdin); 
    if (sscanf(buffer,"%d",&chosen_input) == 1){ //we use sscanf to read from the string buffer for a number. if it is not 1, it means it hasn't found a number.
      if (1 <= chosen_input && chosen_input <= 3){ // the number that was introduced was correct (between 1 and 3)
        return all_location_inputs[chosen_input-1];// -1 because the options start at 1!
      }
      else { // the number provided was not between 1 and 3
        printf("the number provided (%d) was not between 1 and 3. \n", chosen_input);
      }
    }
    else {
    printf("Your input has no number. Try again!\n");
    }
  }
}