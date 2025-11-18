#include "Simulator.h"


#include <chrono>
#include <iostream>


namespace clob {


Simulator::Simulator(MatchingEngine &engine, const SimulatorConfig &cfg)
: engine_(engine), cfg_(cfg), rng_(cfg.seed) {
    // ensure books exist
    for (const auto &s : cfg_.symbols) engine_.ensure_book(s);
}


Simulator::~Simulator() { stop(); }


void Simulator::start() {
    if (running_) return;
    running_ = true;
    th_ = std::thread(&Simulator::run_loop, this);
}


void Simulator::stop() {
    if (!running_) return;
    running_ = false;
    if (th_.joinable()) th_.join();
}


bool Simulator::running() const { return running_; }


void Simulator::run_loop() {
    std::uniform_int_distribution<int> side_dist(0, 1);
    std::uniform_int_distribution<int> sym_idx(0, (int)cfg_.symbols.size() - 1);
    std::uniform_real_distribution<double> price_move(-0.75, 0.75);
    std::uniform_int_distribution<uint64_t> qty_dist(1, cfg_.max_quantity);


    double base = cfg_.base_price;


    while (running_) {
        for (int i = 0; i < cfg_.orders_per_tick; ++i) {
            Order o;
            o.id = next_order_id_.fetch_add(1);
            o.side = (side_dist(rng_) == 0) ? Side::BUY : Side::SELL;
            o.trader_id = "T" + std::to_string(o.id % 100);
            o.symbol = cfg_.symbols[sym_idx(rng_)];
            double move = price_move(rng_);
            // small price deviation around base
            o.price = std::max(0.01, base + move);
            o.quantity = qty_dist(rng_);
            o.initial_quantity = o.quantity;
            o.timestamp = std::chrono::steady_clock::now();


            engine_.submit_order(o);
        }


        std::this_thread::sleep_for(std::chrono::milliseconds(cfg_.tick_ms));
    }
}


} // namespace clob