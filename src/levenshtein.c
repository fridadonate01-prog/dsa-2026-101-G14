#include "levenshtein.h"
#include <string.h>
int levenshtein_distance(char *input_str, char *database_str){
    int inp_len = strlen(input_str);
    int dat_len = strlen(database_str);

    int matrix[inp_len+1][dat_len+1];


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
            matrix[i][j] = min_three(matrix[i-1][j]+1, // deletion
                matrix[i][j-1] + 1, // insertion
                matrix[i-1][j-1] + cost // substitution
            );

        }
    }
    return matrix[inp_len][dat_len];


}

int min_three (int a, int b, int c){
    if (a <= b && a <= c){return a;}
    else if (b <= a && b <= c){return b;}
    return c;
}

