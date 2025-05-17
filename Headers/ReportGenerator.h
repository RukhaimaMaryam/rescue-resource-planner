#pragma once
#include "TransportationNetwork.h"
#include "ResourceManager.h"
#include <iostream>
#include <fstream>
#include <string>

class ReportGenerator {
    TransportationNetwork& network;      // Reference to the transportation network for status queries
    ResourceManager& resourceManager;    // Reference to the resource manager for resource data and critical level checks

public:
    // Constructor initializes references to network and resource manager
    ReportGenerator(TransportationNetwork& net, ResourceManager& rm)
        : network(net), resourceManager(rm) {
    }

    /**
     * @brief Generates and prints a daily status report summarizing network and resource status.
     * @param day The day number to identify the report.
     */
    void generateDailyStatusReport(int day) {
        std::cout << "\n========== DAY " << day << " STATUS REPORT ==========\n";

        // Network status summary: count operational locations connected to central warehouse (ID = 1)
        std::cout << "\nNetwork Summary:\n";
        int operationalLocations = 0;
        int totalLocations = 0;

        // Get all edges from central warehouse (location 1)
        auto edgesFromCentral = network.getEdges(1);
        for (const auto& edge : edgesFromCentral) {
            Location* loc = network.getLocation(edge.to);
            if (loc) {
                totalLocations++;
                if (loc->isOperational) {
                    operationalLocations++;
                }
            }
        }

        // Report on operational vs total locations
        std::cout << "  Operational Locations: " << operationalLocations << "/" << totalLocations << "\n";

        // Check resource critical levels and print warnings or alerts as needed
        resourceManager.checkCriticalLevels();
    }

    /**
     * @brief Prints resource utilization details for a given set of locations.
     *
     * Assumes location IDs are from 1 to N; currently hardcoded for 3 locations.
     * Consider extending this method to dynamically get all locations from the network.
     */
    void generateResourceUtilizationReport() {
        std::cout << "\n========== RESOURCE UTILIZATION REPORT ==========\n";

        // Define which locations to report on; here fixed as 1,2,3 for simplicity
        constexpr int minLocationId = 2;
        constexpr int maxLocationId = 5;

        for (int locationId = minLocationId; locationId <= maxLocationId; ++locationId) {
            Location* loc = network.getLocation(locationId);
            if (loc) {
                std::cout << "Location ID: " << locationId << " (" << loc->name << ")\n";
                loc->printInventory();
                std::cout << "----------------------------------------\n";
            }
            else {
                // Report missing or invalid location info
                std::cout << "Location ID: " << locationId << " - No data available.\n";
            }
        }
    }

    /**
     * @brief Saves combined reports (daily status, resource utilization, and resource manager inventory/allocations)
     *        into a specified file.
     * @param filename Name of the file where the report should be saved.
     */
    void saveReportToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file '" << filename << "' for writing.\n";
            return;
        }

        // Redirect std::cout output to the file stream temporarily
        std::streambuf* originalCoutBuffer = std::cout.rdbuf();
        std::cout.rdbuf(file.rdbuf());

        // Generate reports into the file
        generateDailyStatusReport(0);        // Day 0 or summary report
        generateResourceUtilizationReport();
        resourceManager.printInventory();
        resourceManager.printAllocations();

        // Restore std::cout output back to the console
        std::cout.rdbuf(originalCoutBuffer);

        std::cout << "Report successfully saved to '" << filename << "'.\n";
    }
};
