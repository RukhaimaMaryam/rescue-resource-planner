# Disaster Resource Allocation System

A C++ simulation system for disaster response resource management. It efficiently handles emergency requests using a priority queue (max-heap), tracks resource inventories at multiple locations with hash tables, and finds optimal paths between locations using Dijkstra's algorithm on a graph structure. The system ensures fast lookups, dynamic priority updates, and real-time status management to optimize resource distribution during disasters.

## Features

- **Priority Request Queue:** Manage and process emergency requests based on priority.
- **Location Resource Management:** Track resource inventories and capacities with constant-time lookups using hash tables.
- **Graph-based Pathfinding:** Use adjacency lists and Dijkstra’s algorithm for shortest path calculation between locations.
- **Dynamic Updates:** Support priority changes and status updates for requests and locations during runtime.

## Data Structures Used

- Heap (Max-Heap) for priority management
- Hash Tables (`unordered_map`) for fast resource tracking and lookup
- Graph (Adjacency List) for network connectivity and routing
- Trees for managing priorities

## How to Use

1. Add requests to the priority queue.
2. Allocate and update resources at locations.
3. Process requests in priority order.
4. Use graph algorithms to determine optimal paths for resource delivery.
