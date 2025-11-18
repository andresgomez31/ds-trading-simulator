#pragma once


#include "Order.h"
#include "MatchingEngine.h"


#include <atomic>
#include <functional>
#include <random>
#include <string>
#include <thread>
#include <vector>


namespace clob {


struct SimulatorConfig {
    std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOG"};
    double base_price = 100.0;
    uint64_t max_quantity = 500;
    int orders_per_tick = 5;
    int tick_ms = 200;
    uint64_t seed = 42;
};


class Simulator {
    public:
        Simulator(MatchingEngine &engine, const SimulatorConfig &cfg = {});
        ~Simulator();


        void start();
        void stop();
        bool running() const;


    private:
        void run_loop();


        MatchingEngine &engine_;
        SimulatorConfig cfg_;
        std::atomic<bool> running_ = false;
        std::thread th_;
        std::mt19937_64 rng_;
        std::atomic<uint64_t> next_order_id_{1};
};


} // namespace clob