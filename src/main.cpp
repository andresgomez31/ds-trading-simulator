#include "MatchingEngine.h"
#include "Renderer.h"
#include "Simulator.h"


#include <csignal>
#include <iostream>
#include <memory>
#include <vector>


static clob::Simulator *g_sim = nullptr;


void handle_sigint(int) {
    if (g_sim) g_sim->stop();
    std::exit(0);
}


int main(int argc, char **argv) {
    clob::MatchingEngine engine;


    clob::SimulatorConfig scfg;
    scfg.symbols = {"AAPL", "MSFT", "GOOG"};
    scfg.base_price = 100.0;
    scfg.max_quantity = 200;
    scfg.orders_per_tick = 8;
    scfg.tick_ms = 150;
    scfg.seed = 123456;


    clob::Simulator sim(engine, scfg);
    g_sim = &sim;


    sim.start();


    clob::RendererConfig rcfg;
    rcfg.depth_rows = 12;
    rcfg.refresh_ms = 200;
    rcfg.symbols = scfg.symbols;


    clob::Renderer renderer(engine, rcfg);


    std::signal(SIGINT, handle_sigint);
    renderer.run();


    sim.stop();
    return 0;
}