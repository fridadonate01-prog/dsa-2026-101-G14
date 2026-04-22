#include "find.h"
int levenshtein_distance(char *input_str, char *database_str);
int min_three (int a, int b, int c);

typedef struct {
    char name[256];
    int distance;
} Options;
char * similar_streets(char *inp_string, House *head);
int option_menu(int low_lim, int high_lim);
