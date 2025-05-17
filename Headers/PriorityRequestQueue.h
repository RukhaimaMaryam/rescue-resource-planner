#pragma once
#include "Request.h"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <stdexcept>

class PriorityRequestQueue {
private:
    std::vector<Request> heap;                          // Max-heap based on request priority
    std::unordered_map<int, int> requestIndexMap;       // Maps requestId to index in heap

    // Maintain heap property after insertion or priority increase
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[parent].priority >= heap[index].priority) break;

            std::swap(heap[index], heap[parent]);
            requestIndexMap[heap[index].requestId] = index;
            requestIndexMap[heap[parent].requestId] = parent;

            index = parent;
        }
    }

    // Maintain heap property after removal or priority decrease
    void heapifyDown(int index) {
        int size = static_cast<int>(heap.size());
        while (true) {
            int largest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if (left < size && heap[left].priority > heap[largest].priority)
                largest = left;
            if (right < size && heap[right].priority > heap[largest].priority)
                largest = right;

            if (largest == index) break;

            std::swap(heap[index], heap[largest]);
            requestIndexMap[heap[index].requestId] = index;
            requestIndexMap[heap[largest].requestId] = largest;

            index = largest;
        }
    }

public:
    // Insert a new request into the priority queue
    void addRequest(const Request& req) {
        heap.push_back(req);
        int index = static_cast<int>(heap.size()) - 1;
        requestIndexMap[req.requestId] = index;
        heapifyUp(index);
    }

    // Get the highest-priority request without removing it
    Request getTopRequest() const {
        if (heap.empty()) throw std::runtime_error("Queue is empty");
        return heap.front();
    }

    // Process and remove the top-priority request
    void processTopRequest() {
        if (heap.empty()) throw std::runtime_error("Queue is empty");

        requestIndexMap.erase(heap[0].requestId); // Remove mapping for top

        if (heap.size() == 1) {
            heap.pop_back();
            return;
        }

        heap[0] = heap.back();
        requestIndexMap[heap[0].requestId] = 0;
        heap.pop_back();

        heapifyDown(0);
    }

    // Update the priority of a specific request
    void updateRequestPriority(int requestId, int newPriority) {
        if (!requestIndexMap.count(requestId)) return;

        int index = requestIndexMap[requestId];
        int oldPriority = heap[index].priority;
        heap[index].priority = newPriority;

        if (newPriority > oldPriority)
            heapifyUp(index);
        else if (newPriority < oldPriority)
            heapifyDown(index);
    }

    // Access a specific request by its ID
    Request* getRequest(int requestId) {
        if (!requestIndexMap.count(requestId)) return nullptr;
        return &heap[requestIndexMap[requestId]];
    }

    // Update status (PENDING, COMPLETED, CANCELLED) of a request
    void updateRequestStatus(int requestId, Request::Status newStatus) {
        if (!requestIndexMap.count(requestId)) return;
        heap[requestIndexMap[requestId]].updateStatus(newStatus);
    }

    // Mark a request as cancelled
    void cancelRequest(int requestId) {
        updateRequestStatus(requestId, Request::Status::CANCELLED);
    }

    // Check if the queue is empty
    bool isEmpty() const {
        return heap.empty();
    }

    // Display all requests in the queue
    void printAllRequests() const {
        std::cout << "\n========== All Pending Requests ==========\n";
        std::cout << std::left
            << std::setw(5) << "ID"
            << std::setw(10) << "Priority"
            << std::setw(10) << "Status"
            << std::setw(10) << "Type"
            << std::setw(10) << "Source"
            << std::setw(10) << "Target"
            << std::setw(15) << "Resource"
            << std::setw(10) << "Quantity"
            << std::setw(20) << "Timestamp" << "\n";
        std::cout << std::string(100, '-') << "\n";

        for (const auto& req : heap) {
            std::cout << std::setw(5) << req.requestId
                << std::setw(10) << req.priority
                << std::setw(10) << Request::statusToString(req.status)
                << std::setw(10) << Request::typeToString(req.type)
                << std::setw(10) << req.sourceLocationId
                << std::setw(10) << req.targetLocationId
                << std::setw(15) << req.resourceType
                << std::setw(10) << req.requiredQuantity
                << std::setw(20) << req.timestamp << "\n";
        }
    }
};
