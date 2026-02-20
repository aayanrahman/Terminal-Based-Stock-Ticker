#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include <curl/curl.h> // CHANGE 1: Using native C++ networking instead of OS system calls

// ANSI Escape Codes for UI
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string DIM = "\033[2m";
const std::string CYAN = "\033[36m";
const std::string GREEN = "\033[1;32m";
const std::string RED = "\033[1;31m";
const std::string YELLOW = "\033[1;33m";
const std::string GRAY = "\033[90m";

// CHANGE 2: Instead of clearing the screen, we move the cursor UP 7 lines and to the beginning of the line.
// This overwrites the old box, completely eliminating terminal stacking and flickering.
const std::string RESET_CURSOR = "\033[7A\033[G";
const std::string CLEAR_SCREEN = "\033[2J\033[H"; // Only used for the very first render

// --- NETWORK LAYER ---

// Libcurl requires a "callback" function to handle the data it receives from the internet
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// CHANGE 3: We create a persistent "connection handle".
// By reusing this, we skip the 250ms TLS handshake on every loop (HTTP Keep-Alive).
std::string fetch_binance_price(CURL* curl) {
    std::string readBuffer;
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.binance.com/api/v3/ticker/price?symbol=BTCUSDT");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // This is the secret to low latency: keep the TCP connection alive
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

        // Execute the request
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) return "";
    }
    return readBuffer;
}

// --- PARSER LAYER ---
double parse_price(const std::string& json) {
    std::string target = "\"price\":\"";
    size_t pos = json.find(target);
    if (pos == std::string::npos) return -1.0;

    pos += target.length();
    return std::stod(json.substr(pos));
}

// --- MAIN LOOP ---
int main() {
    using namespace std::chrono;

    // Initialize libcurl (done only once at startup)
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    double last_price = 0.0;
    bool first_run = true;

    std::cout << CLEAR_SCREEN; // Clear the terminal history once at the very start

    while (true) {
        // Start the microsecond stopwatch
        auto start = steady_clock::now();

        // Fetch and parse using the persistent connection
        std::string raw_data = fetch_binance_price(curl);
        double current_price = parse_price(raw_data);

        // Stop the stopwatch
        auto end = steady_clock::now();
        auto latency = duration_cast<milliseconds>(end - start).count();

        // Momentum logic
        std::string price_color = BOLD;
        std::string trend_icon = " ";

        if (last_price != 0.0 && current_price != -1.0) {
            if (current_price > last_price) { price_color = GREEN; trend_icon = "▲"; }
            else if (current_price < last_price) { price_color = RED; trend_icon = "▼"; }
            else { price_color = YELLOW; trend_icon = "−"; }
        }

        // Render UI
        if (!first_run) std::cout << RESET_CURSOR; // Overwrite the previous frame

        std::cout << GRAY << "╭───────────────────────────────────────╮\n" << RESET;
        std::cout << GRAY << "│  " << BOLD << CYAN << "TERMINAL MARKETS" << RESET << GRAY << " (LIVE STATUS)        │\n";
        std::cout << GRAY << "├───────────────────────────────────────┤\n" << RESET;

        if (current_price == -1.0) {
            std::cout << GRAY << "│  " << RESET << "Status:  " << RED << "Connection Error!" << GRAY << "           │\n" << RESET;
        } else {
            std::cout << GRAY << "│  " << RESET << "Asset:   " << BOLD << "Bitcoin (BTC)" << GRAY << "               │\n" << RESET;
            std::cout << GRAY << "│  " << RESET << "Price:   " << price_color << "$" << std::fixed << std::setprecision(2) << current_price << " " << trend_icon << GRAY << "         │\n" << RESET;
        }

        std::cout << GRAY << "│  " << RESET << "Latency: " << DIM << latency << " ms" << GRAY << "                    │\n" << RESET;
        std::cout << GRAY << "╰───────────────────────────────────────╯\n" << RESET;
        std::cout << std::flush;

        if (current_price != -1.0) last_price = current_price;
        first_run = false;

        // 500ms sleep
        std::this_thread::sleep_for(milliseconds(500));
    }

    // Cleanup memory when the program exits
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}