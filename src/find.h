#ifndef FIND_H
#define FIND_H
#define MAX_ID 1000 //revisar

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

typedef struct {
    int id;
    double lat;
    double lon;
} Node;

typedef struct Street{
    Node start;
    Node end;
    int length;
    char street_name[100];
    struct Street *next;
} Street;

typedef struct StreetNode{
    Street* street;
    struct StreetNode* next;
} StreetNode; //for the linked list of streets inside a GridBox

typedef struct GridBox{
    int row, col;
    StreetNode* streets; //"head" of the linked list of streets inside this GridBox
    struct GridBox* next;
} GridBox;

;

//DECLARATIONS:
House* load_houses(const char* f);
Place* load_places(const char* f);
void find_address(House* head);
void find_place(Place* head);

#endif