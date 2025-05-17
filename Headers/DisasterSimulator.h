#pragma once
#include "EventLogger.h"
#include "ResourceManager.h"
#include "TransportationNetwork.h"
#include <random>
#include <iostream>

class DisasterSimulator {
    TransportationNetwork& network;  // Reference to the transportation network to simulate disruptions on
    std::mt19937 rng;                // Mersenne Twister random number generator for reproducible randomness
    EventLogger& logger;             // Reference to the event logger to record simulation events

public:
    // Constructor initializes references and seeds the random number generator with a non-deterministic seed
    DisasterSimulator(TransportationNetwork& net, EventLogger& log)
        : network(net), logger(log) {
        std::random_device rd;      // Obtain seed from hardware entropy source if available
        rng.seed(rd());             // Seed the RNG
    }

    // Simulates a disruption on one of the edges connected to the central warehouse (location ID = 1)
    void simulateNetworkDisruption() {
        // Collect all edges originating from the central warehouse location (ID = 1)
        std::vector<std::pair<int, int>> edges;
        for (const auto& edge : network.getEdges(1)) {
            edges.emplace_back(1, edge.to); // Store edge as (source, target)
        }

        if (edges.empty()) {
            // No edges to disrupt, so exit early
            return;
        }

        // Randomly select one edge to disrupt using uniform distribution
        std::uniform_int_distribution<size_t> edgeDist(0, edges.size() - 1);
        const auto& selectedEdge = edges[edgeDist(rng)];

        // Mark the selected edge as non-operational (disrupted) in both directions if applicable
        network.updateEdgeStatus(selectedEdge.first, selectedEdge.second, false);

        // Log the disruption event
        logger.logNetworkChange(selectedEdge.first, selectedEdge.second, false);

        // Inform console users of the disruption
        std::cout << "\n[DISASTER] Route between locations " << selectedEdge.first
            << " and " << selectedEdge.second << " has been disrupted!\n";
    }

    // Runs a random disaster event chosen from network disruption, resource shortage, or location disruption
    void runRandomEvent(ResourceManager& rm) {
        std::uniform_int_distribution<int> eventTypeDist(0, 2);
        int eventType = eventTypeDist(rng);

        switch (eventType) {
        case 0:
            simulateNetworkDisruption();
            break;
        case 1:
            simulateResourceShortage(rm);
            break;
        case 2:
            simulateLocationDisruption();
            break;
        default:
            // Should never happen, but included for completeness
            break;
        }
    }

    // Simulates a shortage for a randomly selected resource by reducing its available quantity
    void simulateResourceShortage(ResourceManager& rm) {
        // Define the set of possible resource types to affect
        static const std::vector<std::string> resourceTypes = { "Medical Kits", "Water", "Emergency Food", "Blankets", "Medicines" };

        // Select a random resource type
        std::uniform_int_distribution<size_t> typeDist(0, resourceTypes.size() - 1);
        std::string type = resourceTypes[typeDist(rng)];

        // Get a pointer to the Resource object
        Resource* res = rm.getResource(type);
        if (!res) {
            // Resource not found, cannot simulate shortage
            return;
        }

        // Randomly determine shortage percentage between 10% and 30%
        std::uniform_int_distribution<int> percentDist(10, 30);
        int reductionPercent = percentDist(rng);

        // Calculate the amount to reduce based on current availability
        int reductionAmount = static_cast<int>(res->getAvailableQuantity() * reductionPercent / 100.0);

        // Only proceed if the reduction is positive
        if (reductionAmount > 0) {
            res->consume(reductionAmount); // Reduce the resource availability

            // Log the shortage event with detailed info
            logger.log("Resource shortage: " + type + " reduced by "
                + std::to_string(reductionAmount) + " units ("
                + std::to_string(reductionPercent) + "%)");

            // Notify users on console
            std::cout << "\n[DISASTER] " << type << " shortage! Lost "
                << reductionAmount << " units (" << reductionPercent << "%)\n";
        }
    }

    // Simulates a disruption at a randomly selected location (excluding the central warehouse)
    void simulateLocationDisruption() {
        // Gather all locations connected to the central warehouse except itself
        std::vector<int> locationIds;
        for (const auto& edge : network.getEdges(1)) {
            if (edge.to != 1) {
                locationIds.push_back(edge.to);
            }
        }

        if (locationIds.empty()) {
            // No valid locations to disrupt
            return;
        }

        // Randomly select one location from the list
        std::uniform_int_distribution<size_t> locDist(0, locationIds.size() - 1);
        int selectedLocationId = locationIds[locDist(rng)];

        // Retrieve the Location object pointer
        Location* loc = network.getLocation(selectedLocationId);
        if (!loc || !loc->isOperational) {
            // Either invalid pointer or already disrupted location, skip
            return;
        }

        // Mark the location as non-operational
        loc->updateStatus(false);

        // Log this disruption event with location name and ID
        logger.log("Location " + std::to_string(selectedLocationId) + " (" + loc->name + ") is now OFFLINE");

        // Notify via console output
        std::cout << "\n[DISASTER] Location " << selectedLocationId
            << " (" << loc->name << ") is now OFFLINE\n";
    }
};
