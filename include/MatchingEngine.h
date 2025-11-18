#pragma once


#include "OrderBook.h"
#include "Order.h"
#include "Trade.h"


#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>


namespace clob {


class MatchingEngine {
    public:
        MatchingEngine() = default;


        // Submit a new order; returns immediate trades if any
        std::vector<Trade> submit_order(const Order &order);


        // Ensure an order book exists for a symbol
        void ensure_book(const std::string &symbol);


        // For UI: access book aggregated levels and recent trades
        std::vector<PriceLevel> book_top_bids(const std::string &symbol, size_t n) const;
        std::vector<PriceLevel> book_top_asks(const std::string &symbol, size_t n) const;
        std::vector<Trade> book_recent_trades(const std::string &symbol, size_t n) const;


    private:
        mutable std::mutex mtx_;
        std::map<std::string, std::unique_ptr<OrderBook>> books_;
};


} // namespace clob