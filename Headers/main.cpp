#include "ResourceAllocationSimulation.h"

#include <iostream>
#include <ctime>
#include <limits>

int main() {
    try {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        std::cout << "=== Resource Allocation System Simulation ===\n";
        std::cout << "Initializing simulation...\n";

        ResourceAllocationSimulation simulation;

        int days = 0;
        while (true) {
            std::cout << "Enter number of days to simulate (1-10): ";
            if (!(std::cin >> days)) {
                std::cout << "Invalid input. Please enter a number between 1 and 10.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            if (days < 1 || days > 10) {
                std::cout << "Please enter a number between 1 and 10.\n";
                continue;
            }
            break;
        }

        simulation.runSimulation(days);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
