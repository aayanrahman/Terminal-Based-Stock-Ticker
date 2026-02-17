// Aayan Rahman

/*
 * Exchange feed -> c++ executable
 * c++ executable -> terminal
 * Reduces bloat and friction can save time
 */

// To measure the speed as a latency monitor


// Main loop
// 1. Update: Fetch latest price
// 2. Calculate: Figure out the latency and price change %.
// 3. Render: Draw the new data to the terminal.
// 4. Sleep: Wait for X milliseconds (don't max out your CPU!).

// Essential functions
// std::chrono::high_resolution_clock: Use this for your latency measurements.
// std::thread: Eventually, you’ll want your "Networking" to happen on a different thread than your "UI" so the app doesn't freeze while waiting for the internet.
// Namespaces: Wrap your code in namespace ticker { ... } to avoid naming conflicts with libraries.

#include <iostream>
#include "network.hpp"
#include "ui.hpp"
#include "parser.hpp"

int main() {
    // 1. Setup/Initialization
    std::cout << "Starting Aayan's High-Perf Ticker..." << std::endl;

    // 2. The Main Loop
    while (true) {
        // TODO: record start time

        // TODO: call fetch_market_data()

        // TODO: parse the JSON

        // TODO: record end time and calculate latency

        // TODO: Update the UIs

        // Prevent 100% CPU usage with a tiny sleep
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}