#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "find.h"
#include "route.h"

// Helper function to print test results cleanly
void print_test_result(const char* test_name, int condition) {
    if (condition) {
        printf("[ PASSED ] %s\n", test_name);
    } else {
        printf("[ FAILED ] %s\n", test_name);
    }
}

int main() {
    printf("=== STARTING ROUTE MODULE UNIT TESTS ===\n\n");

    // SETUP: CREATE A MOCK MAP DATABASE
    // We define 4 Nodes forming an L-shape
    Node n1 = {1, 41.3851, 2.1734}; // Node 1 (Base corner)
    Node n2 = {2, 41.3851, 2.1800}; // Node 2 (Turning corner)
    Node n3 = {3, 41.3950, 2.1800}; // Node 3 (Top point - creates a LEFT turn from n1->n2)
    Node n4 = {4, 40.0000, 1.0000}; // Node 4 (An isolated point far away)

    // Street A (Horizontal)
    Street sA = {n1, n2, (n1.lat+n2.lat)/2.0,(n1.lon+n2.lon)/2.0, 500, "Street A", 0, NULL};
    // Street B (Vertical, connects to sA at Node 2)
    Street sB = {n2, n3, (n2.lat+n3.lat)/2.0,(n2.lon+n3.lon)/2.0,800, "Street B", 0, NULL};
    // Street C (Isolated street, completely disconnected)
    Street sC = {n4, n4, (n4.lat+n4.lat)/2.0,(n4.lon+n4.lon)/2.0,100, "Isolated Lane", 0, NULL};

    // Link them together into our all_streets linked list
    int test_graph_size = 10;
    IntersectionBucket** test_graph = calloc(test_graph_size, sizeof(IntersectionBucket*));
    
    street_to_intersection(test_graph, test_graph_size, sA.start.id, &sA);
    street_to_intersection(test_graph, test_graph_size, sA.end.id, &sA);
    
    street_to_intersection(test_graph, test_graph_size, sB.start.id, &sB);
    street_to_intersection(test_graph, test_graph_size, sB.end.id, &sB);
    
    street_to_intersection(test_graph, test_graph_size, sC.start.id, &sC);
    street_to_intersection(test_graph, test_graph_size, sC.end.id, &sC);

    Street* map_head = &sA;

    // TEST 1: DIRECT DESTINATION (ORIGIN == DESTINATION)
StreetNode* test1_route = find_route(&sA, &sA, map_head, test_graph, test_graph_size);    int t1_ok = (test1_route != NULL && test1_route->street == &sA && test1_route->next == NULL);
    print_test_result("Test1: Route where Origin is identical to Destination", t1_ok);
    free_path(test1_route);

    // TEST 2: SUCCESSFUL MULTI-STREET PATHFINDING (BFS)
StreetNode* test2_route = find_route(&sA, &sB, map_head, test_graph, test_graph_size);    int t2_ok = (test2_route != NULL && test2_route->street == &sA && test2_route->next != NULL && test2_route->next->street == &sB);
    print_test_result("Test 2: Standard BFS Pathfinding between connected streets", t2_ok);
    free_path(test2_route);

    // TEST 3: UNREACHABLE DESTINATION
StreetNode* test3_route = find_route(&sA, &sC, map_head, test_graph, test_graph_size);    int t3_ok = (test3_route == NULL);
    print_test_result("Test 3: Aborts cleanly when target street is completely isolated", t3_ok);
    free_path(test3_route);

    // TEST 4: CROSS PRODUCT GEOMETRIC TURN EVALUATION
    // Moving from sA (n1->n2) to sB (n2->n3) is a heading change to the North (Left)
    int turn = get_turn_direction(&sA, &sB);
    int t4_ok = (turn == 1); // 1 represents a verified Left Turn
    print_test_result("Test 4: Geometric Turn Direction Cross Product evaluation (Left Turn)", t4_ok);

    printf("\n===UNIT TESTING COMPLETE ===\n");
    for (int i = 0; i < test_graph_size; i++) {
        IntersectionBucket* curr_bucket = test_graph[i];
        while (curr_bucket != NULL) {
            IntersectionBucket* temp_bucket = curr_bucket;
            curr_bucket = curr_bucket->next;
            
            StreetNode* curr_sn = temp_bucket->connected_streets;
            while (curr_sn != NULL) {
                StreetNode* temp_sn = curr_sn;
                curr_sn = curr_sn->next;
                free(temp_sn);
            }
            free(temp_bucket);
        }
    }
    free(test_graph);
    return 0;
}