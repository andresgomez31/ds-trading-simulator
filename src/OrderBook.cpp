#include "OrderBook.h"

#include <algorithm>
#include <deque>
#include <iostream>


namespace clob {


OrderBook::OrderBook(const std::string &symbol) : symbol_(symbol) {}


void OrderBook::add_order(const Order &order) {
    std::lock_guard<std::mutex> lk(mtx_);
    if (order.side == Side::BUY) {
        buy_pq_.push(order);
    } else {
        sell_pq_.push(order);
    }
}


std::vector<Trade> OrderBook::match() {
    std::lock_guard<std::mutex> lk(mtx_);
    std::vector<Trade> trades;


    // local copies will not be used; we operate directly on queues while holding lock
    while (!buy_pq_.empty() && !sell_pq_.empty()) {
        Order buy = buy_pq_.top();
        Order sell = sell_pq_.top();


        if (buy.price < sell.price) break; // no match


        // pop both; we'll reinsert if remaining quantity
        buy_pq_.pop();
        sell_pq_.pop();


        uint64_t trade_qty = std::min(buy.quantity, sell.quantity);


        // choose price by resting order (earlier id)
        double trade_price = (buy.id < sell.id) ? buy.price : sell.price;


        Trade t;
        t.trade_id = next_trade_id_++;
        t.symbol = symbol_;
        t.buy_order_id = buy.id;
        t.sell_order_id = sell.id;
        t.price = trade_price;
        t.quantity = trade_qty;
        t.timestamp = std::chrono::steady_clock::now();


        trades.push_back(t);
        record_trade(t);


        // reduce quantities and reinsert remaining
        if (buy.quantity > trade_qty) {
            buy.quantity -= trade_qty;
            buy_pq_.push(buy);
        }
        if (sell.quantity > trade_qty) {
            sell.quantity -= trade_qty;
            sell_pq_.push(sell);
        }
    }
    return trades;
}

void OrderBook::record_trade(const Trade &t) {
    recent_trades_.push_front(t);
    if (recent_trades_.size() > 200) recent_trades_.pop_back();
}


std::vector<PriceLevel> OrderBook::top_bids(size_t n) const {
    std::lock_guard<std::mutex> lk(mtx_);
    std::vector<PriceLevel> levels;
    auto copy = buy_pq_; // copy priority queue


    while (!copy.empty() && levels.size() < n) {
        Order o = copy.top();
        copy.pop();
        // aggregate same price
        double price = o.price;
        uint64_t qty = o.quantity;
        uint64_t count = 1;
        while (!copy.empty() && copy.top().price == price) {
            qty += copy.top().quantity;
            ++count;
            copy.pop();
        }
        levels.push_back(PriceLevel{price, qty, count});
    }
    return levels;
}


std::vector<PriceLevel> OrderBook::top_asks(size_t n) const {
    std::lock_guard<std::mutex> lk(mtx_);
    std::vector<PriceLevel> levels;
    auto copy = sell_pq_; // copy priority queue


    while (!copy.empty() && levels.size() < n) {
        Order o = copy.top();
        copy.pop();
        double price = o.price;
        uint64_t qty = o.quantity;
        uint64_t count = 1;
        while (!copy.empty() && copy.top().price == price) {
            qty += copy.top().quantity;
            ++count;
            copy.pop();
        }
        levels.push_back(PriceLevel{price, qty, count});
    }
    return levels;
}


std::vector<Trade> OrderBook::recent_trades(size_t n) const {
    std::lock_guard<std::mutex> lk(mtx_);
    std::vector<Trade> res;
    size_t cnt = 0;
    for (const auto &t : recent_trades_) {
        res.push_back(t);
        if (++cnt >= n) break;
    }
    return res;
}


std::string OrderBook::symbol() const { return symbol_; }

} // namespace clob