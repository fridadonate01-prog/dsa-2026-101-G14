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
    double id;
    double lat;
    double lon;
} Node;

typedef struct Street{
    Node start;
    Node end;
    double mid_lat;
    double mid_lon;
    double length;
    char street_name[100];
    int visited;
    struct Street *next;
} Street;

//New structures for Spatial Hashing

typedef struct StreetNode{
    Street* street;
    struct StreetNode* next;
} StreetNode; //for the linked list of streets inside a GridBox

typedef struct GridBox{
    int row, col;
    StreetNode* streets; //"head" of the linked list of streets inside this GridBox
    struct GridBox* next;
} GridBox;

//New structure for Intersection Hash Map
typedef struct IntersectionBucket {
    long long intersection_id; // ¡Antes era int!
    StreetNode* connected_streets;
    struct IntersectionBucket* next;
} IntersectionBucket;

typedef struct position {
    double lat;
    double lon;
} Position;


//DECLARATIONS:
void free_houses(House* head);
void free_place(Place* head);
void free_streets(Street* head);
House* load_houses(const char* f);
Place* load_places(const char* f);
void load_streets(const char* f, Street** street_head, Node** nodes_head, int* GridSize);
void street_to_intersection(IntersectionBucket** graph, int graph_size, long long target_id, Street* street);
Street* find_address(House* head, Street* all_streets) ;
Street* find_place(Place* places_list, Street* all_streets);
int intersection_hash(long long target_id, int graph_size);
Street* get_closest_street(double user_lat, double user_lon, Street* all_streets);
double haversine(Position posA, Position posB);
#endif