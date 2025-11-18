#pragma once


#include "MatchingEngine.h"


#include <chrono>
#include <string>


namespace clob {


struct RendererConfig {
    size_t depth_rows = 10; // number of price rows per side
    int refresh_ms = 200; // UI refresh rate
    std::vector<std::string> symbols = {"AAPL"};
};


class Renderer {
    public:
        Renderer(MatchingEngine &engine, const RendererConfig &cfg = {});


        // Start rendering loop (blocking)
        void run();


        // Single frame render (useful for testing)
        void render_frame();


    private:
        MatchingEngine &engine_;
        RendererConfig cfg_;


        // helpers
        void clear_screen() const;
        void print_header(const std::string &symbol) const;
        void print_book_snapshot(const std::string &symbol) const;
        void print_recent_trades(const std::string &symbol) const;


        // ANSI helpers
        static const char* color_red();
        static const char* color_green();
        static const char* color_yellow();
        static const char* color_reset();
};


} // namespace clob