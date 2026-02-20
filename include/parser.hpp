namespace ticker {
    struct MarketData {
        std::string symbol;
        double price;
        double change_24h;
        long long latency; 
    };
}