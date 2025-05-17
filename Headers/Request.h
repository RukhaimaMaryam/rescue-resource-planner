#pragma once
#include "Utilities.h"
#include <string>

/*
    The Request class models a resource-related request in a disaster management system.
    A request can be of type DEMAND, SUPPLY, or TRANSFER, and may come from or go to specific locations.
    Each request can be tracked using its status, quantity, priority, and notes.
*/

class Request {
public:
    // Enums to represent the current state of the request and its type
    enum class Status { PENDING, FULFILLED, PARTIALLY_FULFILLED, INVALID, CANCELLED };
    enum class Type { SUPPLY, DEMAND, TRANSFER };

    // Unique ID for tracking each request
    int requestId;

    // Source and target location IDs — can represent origin/destination in case of transfer
    int sourceLocationId;
    int targetLocationId;

    // Type of resource requested (e.g., "Water", "Medicine")
    std::string resourceType;

    // Amount of the resource that is needed
    int requiredQuantity;

    // How much of the required quantity has been fulfilled so far
    int fulfilledQuantity;

    // Priority of the request — higher priority means more urgency (can help with scheduling algorithms)
    int priority;

    // Current status of the request
    Status status;

    // Type of request (supply, demand, transfer)
    Type type;

    // When the request was created
    std::string timestamp;

    // Optional human-readable notes for logging/traceability
    std::string notes;

    /*
        Constructor to initialize all the major fields.
        By default, a request is assumed to be a DEMAND and its status is PENDING.
    */
    Request(int id, int sourceId, int targetId, const std::string& type, int qty, int prio,
        Type reqType = Type::DEMAND, const std::string& time = getCurrentTimestamp())
        : requestId(id), sourceLocationId(sourceId), targetLocationId(targetId),
        resourceType(type), requiredQuantity(qty), fulfilledQuantity(0),
        priority(prio), status(Status::PENDING), type(reqType), timestamp(time), notes("") {
    }

    // Update the priority level of a request
    void updatePriority(int newPriority) {
        priority = newPriority;
    }

    // Change the status of the request manually
    void updateStatus(Status newStatus) {
        status = newStatus;
    }

    /*
        Called when a part of the request is fulfilled.
        This increases the fulfilled quantity and updates status accordingly.
        This is useful in partial allocation scenarios — an idea rooted in greedy strategies.
    */
    void fulfillPartial(int qty) {
        fulfilledQuantity += qty;

        // Clamp fulfilled quantity to requiredQuantity in case of over-fulfillment
        if (fulfilledQuantity >= requiredQuantity) {
            fulfilledQuantity = requiredQuantity;
            status = Status::FULFILLED;
        }
        else if (fulfilledQuantity > 0) {
            status = Status::PARTIALLY_FULFILLED;
        }
    }

    // Append additional information to the request's notes
    void addNotes(const std::string& newNotes) {
        if (!notes.empty()) notes += "; ";
        notes += newNotes;
    }

    // Convert the enum Status to a string (for display/logging/UI)
    static std::string statusToString(Status s) {
        switch (s) {
        case Status::PENDING: return "PENDING";
        case Status::FULFILLED: return "FULFILLED";
        case Status::PARTIALLY_FULFILLED: return "PARTIAL";
        case Status::INVALID: return "INVALID";
        case Status::CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
        }
    }

    // Convert the enum Type to a string
    static std::string typeToString(Type t) {
        switch (t) {
        case Type::SUPPLY: return "SUPPLY";
        case Type::DEMAND: return "DEMAND";
        case Type::TRANSFER: return "TRANSFER";
        default: return "UNKNOWN";
        }
    }
};
