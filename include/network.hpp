#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>

// this header file will define the contract tells the rest of the app what the networking module can do

namespace ticker {
    // Function to fetch raw data from an API
    std::string fetch_market_data(const std::string& endpoint);

    // Function to measure latency
    long long get_current_time_ms();
}

#endif