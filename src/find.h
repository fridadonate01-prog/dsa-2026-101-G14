#ifndef FIND_H
#define FIND_H

typedef struct House { //Each house has: street_name, house_number, lat and lon.
    char street_name[100];
    int house_number;
    double lat;
    double lon;
    struct House* next; //Pointer to the next house
} House; 

typedef struct Place{
    char id[40];
    char name[100];
    char category[50];
    double latitude;
    double longitude;
    struct Place *next;
} Place;

//DECLARATIONS:
House* load_houses(const char* f);
Place* load_places(const char* f);
void find_address(House* head);
void find_place(Place* head);

#endif