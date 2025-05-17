// DSA concept used = Unordered Maps 

#pragma once
#include <string>
#include <unordered_map>
#include <iomanip>
#include <iostream>
#include <algorithm>

// Each location represents a physical place
// Unique Id and name, placed on map using longitude and latitude 
// can store people/supplies upto max capacity 
// tracks the map of resources like food/water 

class Location {
public:
    int id;
    std::string name;
    double latitude, longitude;
    bool isOperational;
    int maxCapacity;
    int currentOccupancy;
    // hash table is created for mapping resoruces to quantity 
    std::unordered_map<std::string, int> resourceInventory;

    //constructor
    Location(int id, const std::string& name, double lat, double lon,
        bool operational = true, int capacity = 1000)
        : id(id), name(name), latitude(lat), longitude(lon),
        isOperational(operational), maxCapacity(capacity), currentOccupancy(0) {
    }

    //in case of disaster for simulation it updates that whether location is active and can get resources
    void updateStatus(bool operational) { isOperational = operational; }

    // add resources to location if within maxcapacity
    bool allocateSpace(int quantity) {
        if (quantity <= 0) return false;
        // to check if adding quantity will exceed the capacity 
        if (currentOccupancy + quantity <= maxCapacity) {
            currentOccupancy += quantity;
            return true;
        }
        return false;
    }

    // to free occupied spaces 
    void releaseSpace(int quantity) {
        if (quantity <= 0) return;
        // to ensure that occupancy never goes below zero 
        currentOccupancy = std::max(0, currentOccupancy - quantity);
    }

    void addResource(const std::string& type, int quantity) {
        if (quantity <= 0) return ;
        resourceInventory[type] += quantity;
    }

    // only allows the usage if enough quntity is available 
    bool useResource(const std::string& type, int quantity) {
        if (quantity <= 0) return false ;
        // check if that type of resouce exists ans has enough data 
        if (resourceInventory.count(type) && resourceInventory[type] >= quantity) {
            resourceInventory[type] -= quantity;
            return true;
        }
        return false;
    }

    // tells how much resources are remaining 
    // .find() will look up th resource in hash map
    int getAvailableQuantity(const std::string& type) const {
        auto it = resourceInventory.find(type);
        return (it != resourceInventory.end()) ? it->second : 0;
    }

    void printInventory() const {
        std::cout << "\n+----------------- Location Inventory ----------------+\n";
        std::cout << "| " << std::left << std::setw(20) << "Location ID: " << id << " (" << name << ")\n";
        std::cout << "+-----------------------+---------------------+\n";
        std::cout << "| " << std::setw(21) << "Resource Type" << " | " << std::setw(19) << "Quantity" << " |\n";
        std::cout << "+-----------------------+---------------------+\n";
        for (const auto& res : resourceInventory) {
            std::cout << "| " << std::setw(21) << res.first << " | " << std::setw(19) << res.second << " |\n";
        }
        std::cout << "+-----------------------+---------------------+\n";
    }
};
