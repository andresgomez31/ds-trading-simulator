#pragma once


#include "Order.h"
#include "Trade.h"


#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <vector>


namespace clob {


// Aggregated price level for rendering
struct PriceLevel {
    double price;
    uint64_t total_quantity;
    uint64_t orders_count;
};


class OrderBook {
    public:
        explicit OrderBook(const std::string &symbol);


        // Submit an order into the book (does not perform matching by itself)
        void add_order(const Order &order);


        // Try to match orders; returns list of generated trades
        std::vector<Trade> match();


        // For UI: return aggregated top N levels for bids/asks
        std::vector<PriceLevel> top_bids(size_t n) const;
        std::vector<PriceLevel> top_asks(size_t n) const;


        // Get last trades (most recent first)
        std::vector<Trade> recent_trades(size_t n) const;


        // Stats
        std::string symbol() const;


    private:
        mutable std::mutex mtx_; // protects internal structures
        std::string symbol_;


        // Internal order storage: priority queues where comparator implements price-time priority
        struct BuyCmp {
            bool operator()(const Order &a, const Order &b) const {
                if (a.price != b.price) return a.price < b.price;   // Higher price first
                return a.id > b.id;                                 // Earlier ID wins
            }
        };

        struct SellCmp {
            bool operator()(const Order &a, const Order &b) const {
                if (a.price != b.price) return a.price > b.price;   // Lower price first
                return a.id > b.id;                                 // Earlier ID wins
            }
        };


        std::priority_queue<Order, std::vector<Order>, BuyCmp> buy_pq_;
        std::priority_queue<Order, std::vector<Order>, SellCmp> sell_pq_;


        // Recent trades ring (deque-like)
        std::deque<Trade> recent_trades_;
        uint64_t next_trade_id_ = 1;


        // helper functions
        void record_trade(const Trade &t);
};


} // namespace clob