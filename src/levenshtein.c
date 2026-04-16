#include "levenshtein.h"
#include <stdlib.h>
#include <string.h>
int levenshtein_distance(char *input_str, char *database_str){
    int inp_len = strlen(input_str);
    int dat_len = strlen(database_str);

    int **matrix = calloc(inp_len+1, sizeof(int*));
    for (int i = 0; i < inp_len +1; i++){
        matrix[i] = calloc(dat_len+1, sizeof(int));
    }

    for (int i = 0; i < inp_len + 1; i++){
        matrix[i][0] = i;
    }
    for (int j = 0; j < dat_len + 1; j++){
        matrix[0][j] = j;
    }
    int cost;
    for (int i = 1; i < inp_len + 1; i++){
        for (int j = 1; j < dat_len + 1; j++){

            if (input_str[i-1]==database_str[j-1]){
                cost = 0; 
            }
            else{
                cost = 1;
            }

        }
    }

}