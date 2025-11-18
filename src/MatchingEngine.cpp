#include "MatchingEngine.h"


namespace clob {


void MatchingEngine::ensure_book(const std::string &symbol) {
    std::lock_guard<std::mutex> lk(mtx_);
    if (books_.find(symbol) == books_.end()) {
    books_.emplace(symbol, std::make_unique<OrderBook>(symbol));
    }
}


std::vector<Trade> MatchingEngine::submit_order(const Order &order) {
    // ensure book exists
    {
        std::lock_guard<std::mutex> lk(mtx_);
        if (books_.find(order.symbol) == books_.end()) {
            books_.emplace(order.symbol, std::make_unique<OrderBook>(order.symbol));
        }
    }


    OrderBook *book = nullptr;
    {
        std::lock_guard<std::mutex> lk(mtx_);
        book = books_[order.symbol].get();
    }


    // Add order and attempt matching
    book->add_order(order);
    auto trades = book->match();
    return trades;
}


std::vector<PriceLevel> MatchingEngine::book_top_bids(const std::string &symbol, size_t n) const {
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = books_.find(symbol);
    if (it == books_.end()) return {};
    return it->second->top_bids(n);
}


std::vector<PriceLevel> MatchingEngine::book_top_asks(const std::string &symbol, size_t n) const {
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = books_.find(symbol);
    if (it == books_.end()) return {};
    return it->second->top_asks(n);
}


std::vector<Trade> MatchingEngine::book_recent_trades(const std::string &symbol, size_t n) const {
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = books_.find(symbol);
    if (it == books_.end()) return {};
    return it->second->recent_trades(n);
}


} // namespace clob