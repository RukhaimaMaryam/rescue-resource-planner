#pragma once
#include "Location.h"
#include <vector>
#include <unordered_map>
#include <queue>
#include <climits>
using namespace std;

/*
    It is part of graph, connecting nodes. Thw whole network works as:
    Nodes represent Locations (like hospitals, shelters, etc.)
    Edges represent Routes between these Locations.
*/

struct Edge {
    int to;                  // Target location/node ID this edge connects to
    int capacity;            // Max load (e.g., number of supplies or people) that this route can carry
    int currentLoad;         // Load currently on this edge
    int cost;                // Cost to use this edge (e.g., fuel, effort, or risk score)
    bool isOperational;      // Whether this route is currently usable (true = open, false = blocked)
    double distance;         // Physical or weighted distance between locations
    std::string routeType;   // Type of route (e.g., "road", "air", "rail")

    /*
        Constructor to initialize all edge properties.
        `currentLoad` starts at 0 and increases as load is added.
    */
    Edge(int to, int capacity, int cost, bool operational, double dist = 1.0,
        const std::string& type = "road")
        : to(to),
        capacity(capacity),
        currentLoad(0),
        cost(cost),
        isOperational(operational),
        distance(dist),
        routeType(type) {
    }

    /*
        Check if the edge can handle an additional load.
        Returns true only if:
        - The route is currently operational
        - The new total load (existing + additional) does not exceed capacity
    */
    bool canAddLoad(int additionalLoad) const {
        return isOperational && (currentLoad + additionalLoad <= capacity);
    }

    /*
        Attempt to add load to the edge.
        Only adds if the edge is operational and within capacity.
    */
    void addLoad(int load) {
        if (canAddLoad(load)) {
            currentLoad += load;
        }
    }

    /*
        Safely remove load from the edge.
        Load cannot go below 0, so we clamp it
    */
    void removeLoad(int load) {
        currentLoad = std::max(0, currentLoad - load);
    }
};

class TransportationNetwork {
    std::unordered_map<int, std::vector<Edge>> adjList;
    std::unordered_map<int, Location> locations;

public:
    void addEdge(int from, int to, int capacity, int cost, bool operational = true,
        double distance = 1.0, const std::string& routeType = "road") {

        adjList[from].emplace_back(to, capacity, cost, operational, distance, routeType);
        // Add reverse direction for bidirectional routes (with possibly different parameters)
        adjList[to].emplace_back(from, capacity, cost, operational, distance, routeType);
        adjList[from].back().currentLoad = capacity * (20 + rand() % 50) / 100.0;
        adjList[to].back().currentLoad = capacity * (20 + rand() % 50) / 100.0;
    }
    const auto& getLocations() const { return locations; } // Add this accessor
    const std::vector<Edge>& getEdges(int node) const {
        static const std::vector<Edge> empty;
        auto it = adjList.find(node);
        return it != adjList.end() ? it->second : empty;
    }

    void updateEdgeStatus(int from, int to, bool operational) {
        // Update edge from -> to
        auto itFrom = adjList.find(from);
        if (itFrom != adjList.end()) {
            for (auto& edge : itFrom->second) {
                if (edge.to == to) {
                    edge.isOperational = operational;
                    break;
                }
            }
        }
        // Update edge to -> from
        auto itTo = adjList.find(to);
        if (itTo != adjList.end()) {
            for (auto& edge : itTo->second) {
                if (edge.to == from) {
                    edge.isOperational = operational;
                    break;
                }
            }
        }
    }
    void addLoadToEdge(int from, int to, int load) {
        auto it = adjList.find(from);
        if (it != adjList.end()) {
            for (auto& edge : it->second) {
                if (edge.to == to) {
                    edge.addLoad(load);
                    break;
                }
            }
        }
    }
    std::vector<int> findOptimalPath(int source, int destination, int requiredCapacity = 1) const {
        if (adjList.find(source) == adjList.end() || adjList.find(destination) == adjList.end())
            return {};  // Invalid source or destination
        std::unordered_map<int, int> dist;
        std::unordered_map<int, int> prev;
        typedef std::pair<int, int> pq_node;
        std::priority_queue<pq_node, std::vector<pq_node>, std::greater<pq_node>> pq;

        for (const auto& node : adjList) dist[node.first] = INT_MAX;
        dist[source] = 0;
        pq.emplace(0, source);

        while (!pq.empty()) {
            pq_node top = pq.top();
            int currentDist = top.first;
            int u = top.second;
            pq.pop();

            if (u == destination) break;
            if (currentDist > dist[u]) continue;

            auto edgesIt = adjList.find(u);
            if (edgesIt == adjList.end()) continue;

            for (const Edge& e : edgesIt->second) {
                if (!e.isOperational || !e.canAddLoad(requiredCapacity)) continue;
                if (dist.count(e.to) == 0) dist[e.to] = INT_MAX;
                if (dist[e.to] > dist[u] + e.cost) {
                    dist[e.to] = dist[u] + e.cost;
                    prev[e.to] = u;
                    pq.emplace(dist[e.to], e.to);
                }
            }
        }

        std::vector<int> path;
        int at = destination;
        while (prev.count(at) && at != source) {
            path.push_back(at);
            at = prev[at];
        }
        if (at == source) {
            path.push_back(source);
            std::reverse(path.begin(), path.end());
        }
        return path;
    }

    // In the TransportationNetwork class:
    void addLocation(const Location& loc) {
        locations.emplace(loc.id, loc); // Fixes Location default constructor issue
    }

    Location* getLocation(int id) {
        auto it = locations.find(id);
        return it != locations.end() ? &it->second : nullptr;
    }

    const Location* getLocation(int id) const {
        auto it = locations.find(id);
        return it != locations.end() ? &it->second : nullptr;
    }

    bool isLocationOperational(int id) const {
        auto it = locations.find(id);
        return it != locations.end() && it->second.isOperational;
    }

    void printNetworkStatus() const {
        std::cout << "\n========== Network Status ==========\n";
        std::cout << "Locations:\n";
        for (const auto& loc : locations) {
            std::cout << "  ID: " << loc.first
                << ", Name: " << loc.second.name
                << ", Status: " << (loc.second.isOperational ? "Operational" : "Offline")
                //<< ", Occupancy: " << loc.second.currentOccupancy << "/" << loc.second.maxCapacity
                << "\n";
        }

        std::cout << "\nRoutes:\n";
        for (const auto& node : adjList) {
            for (const auto& edge : node.second) {
                std::cout << "  " << node.first << " -> " << edge.to
                    << " [" << edge.routeType << ", "
                    << (edge.isOperational ? "Open" : "Closed") << ", "
                    << edge.currentLoad << "/" << edge.capacity << " capacity, "
                    << edge.cost << " cost, "
                    << edge.distance << " distance]\n";
            }
        }
    }
};
