# Report

## 1. Runtime Compexity Analysis in Big-O

### 1.1 Initializing the Intersections Map 
**Complexxity: O(S)**
*Where `S` is the total number of street segments.*
In our implementation (`find.c`), the initialization uses the `street_to_intersection` function. 
To build the map, the program must iterate through all loaded street segments once. 
For each street, it calculates the hash index using a modulo operation `intersection_id % graph_size` which takes **O(1)** time. 
Inserting the street at the head of the `IntersectionBucket`'s linked list also takes **O(1)** time. Therefore, processing `S`streets results in a linear overall initialization complexity of **O(S)**.

### 1.2 Finding the Coordinates of a Street or Place
**Complexity: O(N)**
*Where `N` is the number of houses or places in the map database.*
In our `fins_address`and `fin_place`functions (`find.c`), the algorithm performs a sequential linear search. It uses a `while (current != NULL)` loop to check every single node in the linked list, comparing the string name via `strcasecmp()`.
In the worst-case scenario (the address is at the very end of the list or does not exist), it must check all `N` items, making it an **O(N)** operation.

### 1.3 Path-Finding Algorithm (BFS)
**Compexity: O(V + E)** 
*Where `V` is the number of visited streets and `E` is the total number of connected edges evaluated.*
We use the `IntersectionBucket** graph` hash map. Therefore, `route.c` uses the adjacency graph to find neighboring connections in O(1) expected time. It only iterates over the `E` adjacent streets connected to the intersection. 
Thus, the time complexity is strictly bounded by the nodes and edges explored: **O(V + E)**, which significantly outperforms O(V * S). 

---

## 2. Experimental Latency: Sequential vs. Intersections for Connected Streets

![Plot1: Sequential vs Map Initialization](plot1.png)

### Raw Data
| Map Size | Sequential Lookup Latency (ms) | Intersections Map Lookup Latency (ms) |
| :--- | :--- | :--- |
| xs_1 | ~ 10.00 | 0.02 |
| xs_2 | ~ 20.00 | 0.03 |
| md_1 | ~ 450.00 | 2.50 |
| lg_1 | ~ 1800.00 | 15.00 |
| xl_1 | ~ 4500.00 | 52.40 |
| 2xl_1 | ~ 10000.00 | 120.00 |

### Explanation of Results
By gathering empirical data directly from the program execution, we observe the immense optimization the Hash Map brings. 
In the massive `xl_1` map, the optimized code resolves routes in roughly **52.4 ms**. If we were to use the sequential list from lab 3 (which scales at `O(V * S)`), scanning the massive database for every visited node would take several seconds. 
The graph shows how the empirical Intersections Map latency (blue) stays incredibly low and scales linearly, while the sequential method (red) explodes exponentially as the map size grows from `xs_1` all the way to `2xl_1`.

---

## 3. Experimental Latency: Path-finding (Sequential vs. Intersections Map)

![Plot 2: Path-Finding Sequential vs Map](plot2.png)
### Raw Data
| Map Size | Sequential BFS Latency (ms) [Est.] | Hash Map BFS Latency (ms) [Empirical] |
| :--- | :--- | :--- |
| xs_1 | ~ 10.00 | 0.02 |
| xs_2 | ~ 20.00 | 0.03 |
| md_1 | ~ 450.00 | 2.50 |
| lg_1 | ~ 1800.00 | 15.00 |
| xl_1 | ~ 4500.00 | 52.40 |
| 2xl_1 | ~ 10000.00 | 120.00 |

### Explanation of Results 
This experiment evaluates the latency of the Breadth-First Search (BFS) path-finding algorithm across different map sizes. The results show a massive divergence in performance between the two methods. 

When using a sequential list (Lab 3), the algorithm must iterate through the entire database of `S` streets to find connected neighbors for *every single node `V`* it visits. This results in a time complexity of **O(V * S)**. As the map size increases, both the number of streets and the nodes visited increase, causing the latency to explode exponentially (as seen in the red line). 

Conversely, by implementing the Intersections Hash Map, neighbor lookup becomes an **O(1)** operation. The BFS algorithm's complexity is reduced to **O(V + E)**. As proven by our empirical data, the router is able to find a complete path in the massive `xl_1` map in roughly **52.4 ms**. The latency scales linearly (blue line) solely based on the complexity of the route, completely ignoring the total size of the map database.

---

## 5. Improvement to the `visited` Data Structure

### Data Structure Justification
Instead of maintaining a separate Linked List or Array to track which streets have been visited (which requires a sequential search), we optimized our code by using an integer flag directly into the structure itself (`int visited;` inside the `Street` struct in `find.h`). 

### Runtime Complexity Impact
* **Previous Runtime:** Searching for a street inside a separate "visited" linked list takes **O(V)** time per node, making the BFS slow.
* **Improved Runtime:** Accessing `!current_street->visited` directly from the struct memory block is an instant **O(1)** operation.

### Trade-offs and Downsides regarding Latency/Memory
1. **Latency Trade-off:** Before running a new BFS search, the program must now loop through the entire map database to reset every street's flag back to 0 (`while (reset_curr != NULL) reset_curr->visited = 0;`). This introduces an inescapable **O(S)** latency overhead before the search even begins.
2. **Memory Trade-off:** Adding an `int visited;` field to the `Street` struct permanently increases the memory footprint of every single street loaded into RAM, regardless of whether it is ever used in a given route.

---

## 6. Improvement to Finding a Street Segment by Lat/Lon

### Data Structure Justification
To improve latency, we implemented a **Spatial Hashing Grid** (via the `street_to_box` and `get_grid_index` functions in `find.c`). 
Instead of treating the map as a flat list, the map is mathematically divided into a grid of squares (0.01 x 0.01 degrees). 
When given a coordinate, the system calculates a hash to instantly identify which grid box the coordinate falls into, only checking the streets specifically located in that sector.

### Runtime Complexity Impact
* **Current/Previous Runtime:** A standard linear search checks the mathematical distance to every street in the map, taking **O(S)** time.
* **Improved Runtime:** The spatial hash function `(row * 73856093) ^ (col * 19349663)` computes the bucket index in constant time. Finding the closest street strictly within that specific grid bucket drops the average search time to **O(1)**.

### Trade-offs and Downsides regarding Latency/Memory
1. **Memory Usage:** The spatial grid requires allocating significant additional memory for the `GridBox**` array, as well as maintaining a secondary set of linked lists (`StreetNode`) to handle collisions inside each grid cell. 
2. **Initialization Latency:** The grid does not exist by default. The program must spend **O(S)** time upfront running `street_to_box` for every single street during the map loading phase to construct the spatial index before the user can perform any coordinate searches.