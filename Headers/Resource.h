#pragma once
#include <string>
#include <algorithm> 

// The Resource class represents a type of resource (e.g., Water, Food, Medicine)
// with its properties like total quantity, cost, expiry, etc.
// It supports operations like allocation, release, consumption, and stock management.

class Resource {
public:
    std::string type;        // Type of resource (e.g., "Water", "Medicine")
    int totalQuantity;       // Total units of this resource in stock
    int allocatedQuantity;   // Units that are currently allocated/reserved
    int expiryDate;          // Optional: expiry date (e.g., YYYYMMDD format)
    double unitCost;         // Cost per unit (useful for optimization or budgeting)
    double weight;           // Weight per unit (used in transport logistics)
    int criticalLevel;       // Threshold below which stock is considered critical

    // Constructor 
    Resource(const std::string& type, int qty, int expiry = 0, double cost = 1.0,
        double weight = 1.0, int criticalLevel = 100)
        : type(type),
        totalQuantity(qty),
        allocatedQuantity(0),
        expiryDate(expiry),
        unitCost(cost),
        weight(weight),
        criticalLevel(criticalLevel) {
    }

    // Tries to allocate 'qty' units of the resource
    // Returns true if successful (enough stock available), false otherwise
    bool allocate(int qty) {
        if (getAvailableQuantity() >= qty) {
            allocatedQuantity += qty;
            return true;
        }
        return false;
    }

    // Releases 'qty' units that were previously allocated
    // Ensures that allocated quantity never goes below zero
    void release(int qty) {
        allocatedQuantity = std::max(0, allocatedQuantity - qty);
    }

    // Consumes 'qty' units of the resource (e.g., used or wasted)
    // Affects both allocated and total stock
    void consume(int qty) {
        allocatedQuantity = std::max(0, allocatedQuantity - qty);
        totalQuantity = std::max(0, totalQuantity - qty);
    }

    // Adds 'qty' new units to the stock (e.g., received from suppliers)
    void addStock(int qty) {
        totalQuantity += qty;
    }

    // Checks if the available (unallocated) quantity has fallen below the critical level
    bool isBelowCriticalLevel() const {
        return getAvailableQuantity() < criticalLevel;
    }

    // Returns the number of units that are available (not allocated)
    int getAvailableQuantity() const {
        return totalQuantity - allocatedQuantity;
    }
};
