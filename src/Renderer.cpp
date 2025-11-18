#include "Renderer.h"


#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

namespace clob {


Renderer::Renderer(MatchingEngine &engine, const RendererConfig &cfg) : engine_(engine), cfg_(cfg) {
    // ensure books exist
    for (const auto &s : cfg_.symbols) engine_.ensure_book(s);
}


void Renderer::run() {
    while (true) {
        render_frame();
        std::this_thread::sleep_for(std::chrono::milliseconds(cfg_.refresh_ms));
    }
}


void Renderer::render_frame() {
    clear_screen();
    for (const auto &sym : cfg_.symbols) {
        print_header(sym);
        print_book_snapshot(sym);
        print_recent_trades(sym);
        std::cout << "\n";
    }
    std::cout << std::flush;
}

void Renderer::clear_screen() const {
    std::stringstream ss;
    ss << "Time: ";
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    ss << std::put_time(std::localtime(&t), "%F %T") << "\n";
    std::cout << ss.str();
}

void Renderer::print_header(const std::string &symbol) const {
    std::cout << "=== Order Book for " << symbol << " ===\n";
}


void Renderer::print_book_snapshot(const std::string &symbol) const {
    auto bids = engine_.book_top_bids(symbol, cfg_.depth_rows);
    auto asks = engine_.book_top_asks(symbol, cfg_.depth_rows);


    size_t rows = std::max(bids.size(), asks.size());
    rows = std::max(rows, cfg_.depth_rows);


    std::cout << " BID (price x qty) | ASK (price x qty)\n";
    std::cout << "-----------------------------------------------------------------\n";
    for (size_t i = 0; i < rows; ++i) {
        // left (bid)
        if (i < bids.size()) {
            const auto &b = bids[i];
            std::cout << color_green();
            std::cout << std::setw(8) << std::fixed << std::setprecision(2) << b.price << " x " << b.total_quantity;
            std::cout << color_reset();
        } else {
            std::cout << std::setw(24) << " ";
        }


        std::cout << " | ";


        if (i < asks.size()) {
            const auto &a = asks[i];
            std::cout << color_red();
            std::cout << std::setw(8) << std::fixed << std::setprecision(2) << a.price << " x " << a.total_quantity;
            std::cout << color_reset();
        }
        std::cout << "\n";
    }
}

void Renderer::print_recent_trades(const std::string &symbol) const {
    auto trades = engine_.book_recent_trades(symbol, 10);
    if (trades.empty()) return;


    std::cout << "Recent trades:\n";
    for (const auto &t : trades) {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t.timestamp.time_since_epoch()).count();
    std::cout << color_yellow();
    std::cout << " id=" << t.trade_id << " qty=" << t.quantity << " @ " << std::fixed << std::setprecision(2) << t.price;
    std::cout << color_reset() << "\n";
    }
}


const char* Renderer::color_red() { return "\033[31m"; }
const char* Renderer::color_green() { return "\033[32m"; }
const char* Renderer::color_yellow() { return "\033[33m"; }
const char* Renderer::color_reset() { return "\033[0m"; }


} // namespace clob