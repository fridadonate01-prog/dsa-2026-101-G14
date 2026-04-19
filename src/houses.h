#ifndef HOUSES_H
#define HOUSES_H

typedef struct House {
    char street_name[100];
    int house_number;
    double lat;
    double lon;
    struct House* next; // Linked list pointer
} House;

// Infrastructure function
House* load_houses(const char* filename);

void find_address_logic(House* head, int choice);

#endif