#pragma once

#include "TransportationNetwork.h"
#include "ResourceManager.h"
#include "PriorityRequestQueue.h"
#include "EventLogger.h"
#include "DisasterSimulator.h"
#include "ReportGenerator.h"

#include <random>
#include <iostream>
#include <vector>
#include <string>

class ResourceAllocationSimulation {
    TransportationNetwork network;
    ResourceManager resourceManager;
    PriorityRequestQueue requestQueue;
    EventLogger logger;
    DisasterSimulator disasterSim;
    ReportGenerator reportGen;

    int simulationDay;
    int nextRequestId;
    std::mt19937 rng;

public:
    ResourceAllocationSimulation()
        : resourceManager(network),
        logger("simulation.log"),
        disasterSim(network, logger),
        reportGen(network, resourceManager),
        simulationDay(1),
        nextRequestId(1),
        rng(std::random_device{}()) // Initialize RNG once here
    {
        initializeSystem();
    }

    void initializeSystem() {
        // Initialize locations with clear info and consistent parameters
        network.addLocation(Location(1, "Central Warehouse", 34.0522, -118.2437, true, 10000));
        network.addLocation(Location(2, "Downtown Hospital", 34.0495, -118.2512, true, 5000));
        network.addLocation(Location(3, "North Shelter", 34.0639, -118.2381, true, 3000));
        network.addLocation(Location(4, "East Medical Center", 34.0500, -118.2000, true, 4000));
        network.addLocation(Location(5, "South Distribution Hub", 34.0300, -118.2400, true, 8000));

        // Configure transportation routes with detailed params and status
        network.addEdge(1, 2, 1000, 5, true, 5.2, "road");   // Warehouse to Hospital
        network.addEdge(1, 3, 800, 8, true, 7.8, "road");    // Warehouse to Shelter
        network.addEdge(1, 4, 1200, 6, true, 6.5, "road");   // Warehouse to East Medical
        network.addEdge(1, 5, 1500, 4, true, 4.2, "road");   // Warehouse to South Hub
        network.addEdge(2, 3, 300, 15, false, 3.1, "road");  // Hospital to Shelter (damaged)
        network.addEdge(2, 4, 400, 12, true, 3.7, "road");   // Hospital to East Medical
        network.addEdge(3, 5, 600, 10, true, 9.3, "road");   // Shelter to South Hub
        network.addEdge(4, 5, 700, 9, true, 5.8, "road");    // East Medical to South Hub

        // Initialize resources with detailed characteristics
        resourceManager.addResource(Resource("Medical Kits", 1000, 365, 50.0, 2.5, 200));
        resourceManager.addResource(Resource("Water", 5000, 90, 2.0, 1.0, 1000));
        resourceManager.addResource(Resource("Emergency Food", 3000, 180, 8.0, 0.75, 500));
        resourceManager.addResource(Resource("Blankets", 800, 0, 15.0, 1.5, 100));
        resourceManager.addResource(Resource("Medicines", 500, 240, 100.0, 0.5, 100));

        // Seed initial inventory at locations safely
        if (auto* hospital = network.getLocation(2)) {
            hospital->addResource("Medical Kits", 200);
            hospital->addResource("Water", 500);
            hospital->addResource("Medicines", 100);
        }

        if (auto* shelter = network.getLocation(3)) {
            shelter->addResource("Water", 300);
            shelter->addResource("Emergency Food", 400);
            shelter->addResource("Blankets", 200);
        }

        // Seed initial requests with clear logging
        requestQueue.addRequest(Request(nextRequestId++, 1, 2, "Medical Kits", 200, 10));
        requestQueue.addRequest(Request(nextRequestId++, 1, 3, "Emergency Food", 500, 8));
        requestQueue.addRequest(Request(nextRequestId++, 1, 2, "Water", 1000, 9));

        logger.log("System initialized with 5 locations, 8 routes, 5 resource types, and 3 initial requests");
    }

    void runSimulation(int totalDays) {
        std::cout << "\n========== Starting Resource Allocation Simulation ==========\n";

        for (; simulationDay <= totalDays; ++simulationDay) {
            std::cout << "\n========== DAY " << simulationDay << " ==========\n";
            logger.log("Beginning of Day " + std::to_string(simulationDay));

            processRequests();
            resourceManager.checkCriticalLevels();

            // 20% chance of disaster event, run only if disasterSim enabled
            if (std::uniform_real_distribution<double>(0.0, 1.0)(rng) < 0.2) {
                disasterSim.runRandomEvent(resourceManager);
            }

            reportGen.generateDailyStatusReport(simulationDay);

            if (simulationDay < totalDays) {
                generateDailyRequests();
            }

            if (simulationDay % 5 == 0) {
                reportGen.saveReportToFile("day_" + std::to_string(simulationDay) + "_report.txt");
            }
        }

        printFinalReport();
    }

private:
    void processRequests() {
        int processedCount = 0;

        if (requestQueue.isEmpty()) {
            std::cout << "No requests to process today.\n";
            return;
        }

        requestQueue.printAllRequests();

        while (!requestQueue.isEmpty()) {
            Request current = requestQueue.getTopRequest();

            std::cout << "\nProcessing Request #" << current.requestId
                << " (" << current.resourceType << " x" << current.requiredQuantity
                << " from Loc" << current.sourceLocationId << " to Loc" << current.targetLocationId << ")\n";

            logger.logRequest(current);

            if (!network.isLocationOperational(current.sourceLocationId) ||
                !network.isLocationOperational(current.targetLocationId)) {
                current.updateStatus(Request::Status::INVALID);
                current.addNotes("One or both locations are not operational");
                std::cout << "Request invalid: One or both locations are not operational!\n";
                requestQueue.processTopRequest();
                continue;
            }

            auto path = network.findOptimalPath(
                current.sourceLocationId, current.targetLocationId, current.requiredQuantity);

            if (path.empty()) {
                current.updateStatus(Request::Status::INVALID);
                current.addNotes("No valid transportation route available");
                std::cout << "No valid transportation route available!\n";
                requestQueue.processTopRequest();
                continue;
            }

            std::cout << "Optimal route found: ";
            for (size_t i = 0; i < path.size(); ++i) {
                std::cout << path[i];
                if (i < path.size() - 1) std::cout << " -> ";
            }
            std::cout << "\n";

            bool resourceAvailable = resourceManager.hasAvailableResource(
                current.resourceType, current.requiredQuantity);

            if (!resourceAvailable) {
                current.updateStatus(Request::Status::INVALID);
                current.addNotes("Insufficient resources available");
                std::cout << "Insufficient resources available!\n";
                requestQueue.processTopRequest();
                continue;
            }

            bool allocationSuccess = resourceManager.allocateResources(
                current.resourceType, current.requiredQuantity,
                current.sourceLocationId, current.targetLocationId);

            if (allocationSuccess) {
                current.updateStatus(Request::Status::FULFILLED);
                current.fulfillPartial(current.requiredQuantity);
                std::cout << "Successfully allocated resources!\n";
                logger.logAllocation(
                    current.sourceLocationId, current.targetLocationId,
                    current.resourceType, current.requiredQuantity, current.timestamp);
            }
            else {
                current.updateStatus(Request::Status::PARTIALLY_FULFILLED);
                std::cout << "Partial allocation: transportation constraints!\n";
            }

            requestQueue.processTopRequest();
            ++processedCount;
        }

        std::cout << "\nProcessed " << processedCount << " requests.\n";
    }

    void generateDailyRequests() {
        std::uniform_int_distribution<int> countDist(1, 3);
        int newRequestCount = countDist(rng);

        std::vector<std::string> resourceTypes = {
            "Medical Kits", "Water", "Emergency Food", "Blankets", "Medicines"
        };
        std::uniform_int_distribution<size_t> typeDist(0, resourceTypes.size() - 1);
        std::uniform_int_distribution<int> locDist(2, 5);
        std::uniform_int_distribution<int> qtyDist(50, 500);
        std::uniform_int_distribution<int> prioDist(3, 10);

        for (int i = 0; i < newRequestCount; ++i) {
            std::string resType = resourceTypes[typeDist(rng)];
            int targetLoc = locDist(rng);
            int qty = qtyDist(rng);
            int priority = prioDist(rng);

            Request newReq(nextRequestId++, 1, targetLoc, resType, qty, priority);
            requestQueue.addRequest(newReq);

            std::cout << "New request generated: #" << newReq.requestId
                << " for " << resType << " x" << qty
                << " to location " << targetLoc
                << " (Priority: " << priority << ")\n";

            logger.logRequest(newReq);
        }
    }

    void printFinalReport() {
        std::cout << "\n========== FINAL SIMULATION REPORT ==========\n";

        network.printNetworkStatus();
        resourceManager.printInventory();
        reportGen.generateResourceUtilizationReport();
        resourceManager.printAllocations();
        reportGen.saveReportToFile("final_simulation_report.txt");

        std::cout << "\n========== Simulation Completed ==========\n";
    }
};
