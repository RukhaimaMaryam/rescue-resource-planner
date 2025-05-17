#pragma once
#include "Resource.h"
#include "TransportationNetwork.h"
#include "Utilities.h"
#include "Request.h"
#include <vector>
#include <tuple>

class ResourceManager {
    std::unordered_map<std::string, Resource> resources;
    std::vector<std::tuple<int, int, std::string, int, std::string>> allocationRecords; // source, target, type, qty, timestamp
    TransportationNetwork& network;
    int nextRequestId = 1;


public:
    ResourceManager(TransportationNetwork& net) : network(net) {}

    bool allocateResources(const std::string& type, int qty, int sourceLocationId, int targetLocationId) {
        auto it = resources.find(type);
        if (it == resources.end()) return false;

        Resource& res = it->second;
        if (res.allocate(qty)) {
            allocationRecords.emplace_back(sourceLocationId, targetLocationId, type, qty, getCurrentTimestamp());

            // Add resources to the target location if available
            Location* targetLoc = network.getLocation(targetLocationId);
            if (targetLoc) {
                targetLoc->addResource(type, qty);
            }

            return true;
        }
        return false;
    }

    bool transferResources(int sourceLocationId, int targetLocationId, const std::string& type, int qty) {
        Location* sourceLoc = network.getLocation(sourceLocationId);
        Location* targetLoc = network.getLocation(targetLocationId);
        Resource* res = getResource(type);

        if (!sourceLoc || !targetLoc || !res) return false;
        if (!sourceLoc->isOperational || !targetLoc->isOperational) return false;

        // Calculate total weight of the transfer
        int totalWeight = qty * res->weight;
        if (totalWeight <= 0) return false;

        // Find path with capacity for the total weight
        auto path = network.findOptimalPath(sourceLocationId, targetLocationId, totalWeight);
        if (path.empty()) return false;

        // Verify all edges in the path can handle the load
        bool canTransfer = true;
        for (size_t i = 0; i < path.size() - 1; ++i) {
            int from = path[i];
            int to = path[i + 1];
            const auto& edges = network.getEdges(from);
            bool edgeFound = false;
            for (const auto& edge : edges) {
                if (edge.to == to && edge.canAddLoad(totalWeight)) {
                    edgeFound = true;
                    break;
                }
            }
            if (!edgeFound) {
                canTransfer = false;
                break;
            }
        }
        if (!canTransfer) return false;

        // Update edge loads
        for (size_t i = 0; i < path.size() - 1; ++i) {
            int from = path[i];
            int to = path[i + 1];
            network.addLoadToEdge(from, to, totalWeight);
        }

        // Proceed with resource transfer
        if (sourceLoc->useResource(type, qty)) {
            targetLoc->addResource(type, qty);
            allocationRecords.emplace_back(sourceLocationId, targetLocationId, type, qty, getCurrentTimestamp());
            return true;
        }
        return false;
    }

    // In the ResourceManager class:
    void addResource(const Resource& res) {
        resources.emplace(res.type, res);
    }
    void printInventory() const {
        std::cout << "\n========== Central Resource Inventory ==========\n";
        std::cout << std::left << std::setw(15) << "Type"
            << std::setw(15) << "Total Qty"
            << std::setw(15) << "Available"
            << std::setw(10) << "Expiry"
            << std::setw(10) << "Cost"
            << std::setw(10) << "Weight"
            << std::setw(10) << "Critical" << "\n";
        std::cout << std::string(85, '-') << "\n";

        for (const auto& entry : resources) {
            const Resource& res = entry.second;
            std::cout << std::setw(15) << entry.first
                << std::setw(15) << res.totalQuantity
                << std::setw(15) << (res.totalQuantity - res.allocatedQuantity)
                << std::setw(10) << (res.expiryDate > 0 ? std::to_string(res.expiryDate) + "d" : "N/A")
                << std::setw(10) << res.unitCost
                << std::setw(10) << res.weight
                << std::setw(10) << (res.isBelowCriticalLevel() ? "YES" : "No")
                << "\n";
        }
    }

    void printAllocations() const {
        std::cout << "\n========== Resource Allocations ==========\n";
        std::cout << std::left << std::setw(15) << "Source"
            << std::setw(15) << "Target"
            << std::setw(15) << "Resource"
            << std::setw(15) << "Quantity"
            << std::setw(20) << "Timestamp" << "\n";
        std::cout << std::string(80, '-') << "\n";

        for (const auto& record : allocationRecords) {
            std::cout << std::setw(15) << std::get<0>(record)  // Source
                << std::setw(15) << std::get<1>(record)  // Target
                << std::setw(15) << std::get<2>(record)  // Resource Type
                << std::setw(15) << std::get<3>(record)  // Quantity
                << std::setw(20) << std::get<4>(record)  // Timestamp
                << "\n";
        }
    }

    void checkCriticalLevels() const {
        bool anyBelowCritical = false;
        std::cout << "\n========== Critical Resources Alert ==========\n";

        for (const auto& entry : resources) {
            const Resource& res = entry.second;
            if (res.isBelowCriticalLevel()) {
                std::cout << "WARNING: " << entry.first << " is below critical level! "
                    << "Available: " << res.getAvailableQuantity()
                    << " (Critical threshold: " << res.criticalLevel << ")\n";
                anyBelowCritical = true;
            }
        }

        if (!anyBelowCritical) {
            std::cout << "All resources are above critical levels.\n";
        }
    }

    Resource* getResource(const std::string& type) {
        auto it = resources.find(type);
        return it != resources.end() ? &it->second : nullptr;
    }

    bool hasAvailableResource(const std::string& type, int quantity) const {
        auto it = resources.find(type);
        return it != resources.end() && (it->second.totalQuantity - it->second.allocatedQuantity) >= quantity;
    }

    int createSupplyRequest(int targetLocationId, const std::string& resourceType, int quantity) {
        Request req(nextRequestId++, 0, targetLocationId, resourceType, quantity, 5, Request::Type::SUPPLY);
        // Process supply request logic would go here
        return req.requestId;
    }
};
