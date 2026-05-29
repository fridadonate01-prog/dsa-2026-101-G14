# Report

## 1. Runtime Compexity Analysis in Big-O

### 1.1 Initializing the Intersections Map 
**Complexxity: O(S)**
*Where `S` is the total number of street segments.*
In our implementation (`find.c`), the initialization uses the `street_to_intersetcion` function. 
To build the map, the program must iterate through all loaded street segments once. 
For each street, it calculates the hash index using a modulo operation `intersection_id % graph_size` which takes **O(1)** time. 
Inserting the street at the head of the `IntersectionBucket`'s linked list also takes **O(1)** time. Therefore, processing `S`streets results in a linear overall initialization complexity of **O(S)**.

### 1.2 Finding the Coordinates of a Street or Place
**Complexity: O(N)**
*Where `N` is the number of houses or places in the map database.*
In our `fins_address`and `fin_place`functions (`find.c`), the algorithm performs a sequential linear search. It uses a `while (current != NULL)` loop to check every single node in the linked list, comparing the string name via `strcasecmp()`.
In the worst-case scenario (the address is at the very end of the list or does not exist), it must check all `N` items, making it an **O(N)** operation.

### 1.3 Path-Finding Algorithm (BFS)
**Compexity: O(V * S)** 
*Where `V` is the number of visited streets and `S` is the total number of streets in the map.*
In our `find_route`function (`route.c`), the BFS uses a queue to explore nodes. However, to find the connected neighbors of the `current_street`, the code performs a sequential scan of the entire map database (`Street* connected_street = all_streets; while(connected_street != NULL)`).
Because it iterates through all `S` streets for every single one of the `V` streets it visits to check for shared intersection IDs, the runtime complexity degrades to **O(V * S)**. 

---

## 2. Experimental Latency: Sequential vs. Intersections 
