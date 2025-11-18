#pragma once


#include <chrono>
#include <cstdint>
#include <string>


namespace clob {


struct Trade {
    uint64_t trade_id;
    std::string symbol;
    uint64_t buy_order_id;
    uint64_t sell_order_id;
    double price;
    uint64_t quantity;
    std::chrono::steady_clock::time_point timestamp;
};


} // namespace clob