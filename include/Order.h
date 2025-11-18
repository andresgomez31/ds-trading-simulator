#pragma once


#include <chrono>
#include <cstdint>
#include <string>


namespace clob {


enum class Side { BUY, SELL };


struct Order {
    uint64_t id;
    Side side;
    std::string trader_id;
    std::string symbol;
    double price;
    uint64_t quantity;
    uint64_t initial_quantity;
    std::chrono::steady_clock::time_point timestamp;


    Order() = default;
    Order(uint64_t id_, Side side_, const std::string &trader,
    const std::string &symbol_, double price_, uint64_t qty_);
};


} // namespace clob