Disaster Resource Allocation System
This C++ project simulates disaster response resource management. It efficiently handles emergency requests using a priority queue (max-heap), tracks resource inventories at various locations with hash tables, and finds optimal paths between locations using Dijkstra's algorithm on a graph structure. The system ensures fast lookups, dynamic priority updates, and real-time status management to optimize resource distribution during disasters.

Features
Priority Request Queue: Manages emergency requests by priority for processing.

Location Management: Tracks resources and occupancy using hash maps for constant-time operations.

Graph-based Pathfinding: Uses adjacency lists and Dijkstra's algorithm for shortest paths.

Dynamic Updates: Supports updating request priorities and location status dynamically.

Data Structures Used
Heap: For priority queue implementation.

Hash Tables (unordered_map): For resource tracking and fast access.

Graph (Adjacency List): For network connectivity and routing.

Trees: For managing priorities in the queue.

Usage
Add requests to the priority queue.

Allocate and update resources at locations.

Process requests in priority order.

Find shortest paths for resource delivery.
