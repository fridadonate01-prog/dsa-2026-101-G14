#include <stdio.h>
#include <string.h>

// 1. The function MUST be declared or defined BEFORE main()
// 2. The parameter must be an array of strings (const char *maps[])
const char* choose_map(const char *maps[]) {
  char chosen_map[50]; 
  
  while (1) { 
    printf("Enter map name (e.g. 'xs_2' or 'xl_1'): \n"); 
    scanf("%49s", chosen_map); 
    
    for (int i = 0; i < 6; i++) {
      // 3. maps[i] is already the string pointer, DO NOT use &maps[i]
      if (strcmp(maps[i], chosen_map) == 0) {
          return maps[i]; // Return the pointer to the matched string
      } 
    }
    printf("Your input (%s) does not match with the possible options! Try again!\n", chosen_map);
  }
}