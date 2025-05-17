#pragma once
#include "Utilities.h"  // For utility functions like getCurrentTimestamp()
#include "Request.h"    // Definition of Request class and related enums
#include <fstream>      // File stream for logging output

class EventLogger {
    std::ofstream logFile; // Output file stream for writing log entries to a file
    bool enabled;          // Flag to indicate if logging is enabled (file opened successfully)

public:
    // Constructor opens the log file in append mode.
    // If the file can't be opened, disables logging and prints a warning.
    EventLogger(const std::string& filename = "resource_allocation.log") : enabled(true) {
        logFile.open(filename, std::ios::app); // Open file in append mode to keep existing logs
        if (!logFile.is_open()) {
            std::cerr << "Warning: Could not open log file: " << filename << "\n";
            enabled = false;  // Disable logging if file open fails
        }
    }

    // Destructor closes the log file if it is open
    ~EventLogger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    // General logging function that writes a timestamped message to the log file
    // and also prints it to the console.
    void log(const std::string& message) {
        if (!enabled) return; // Skip logging if disabled

        std::string timestamp = getCurrentTimestamp(); // Get current timestamp as string
        logFile << "[" << timestamp << "] " << message << std::endl; // Write timestamped message to file

        // Also output the log message to the standard output for real-time monitoring
        std::cout << "[LOG] " << message << std::endl;
    }

    // Logs detailed information about a Request object,
    // including its ID, resource type, quantity, source and target locations, and status.
    void logRequest(const Request& req) {
        if (!enabled) return;

        // Compose a descriptive log message for the request event
        std::string message = "Request #" + std::to_string(req.requestId) +
            " (" + req.resourceType + " x" + std::to_string(req.requiredQuantity) +
            ") from Loc" + std::to_string(req.sourceLocationId) +
            " to Loc" + std::to_string(req.targetLocationId) +
            " - Status: " + Request::statusToString(req.status);

        log(message); // Use the general log function to output the message
    }

    // Logs an allocation event specifying source and target locations,
    // resource type and quantity, and the time of allocation.
    void logAllocation(int sourceId, int targetId, const std::string& resourceType,
        int quantity, const std::string& timestamp) {
        if (!enabled) return;

        // Compose the allocation log message with all relevant details
        std::string message = "Allocated " + resourceType + " x" + std::to_string(quantity) +
            " from Loc" + std::to_string(sourceId) +
            " to Loc" + std::to_string(targetId) +
            " at " + timestamp;

        log(message);
    }

    // Logs changes in network routes between two locations,
    // indicating if the route is operational or closed.
    void logNetworkChange(int from, int to, bool isOperational) {
        if (!enabled) return;

        // Compose a message reflecting the route status change
        std::string message = "Route from Loc" + std::to_string(from) +
            " to Loc" + std::to_string(to) +
            " is now " + (isOperational ? "OPERATIONAL" : "CLOSED");

        log(message);
    }
};
