#include "Order.h"


namespace clob {


Order::Order(uint64_t id_, Side side_, const std::string &trader, const std::string &symbol_, double price_, uint64_t qty_)
    : id(id_), side(side_), trader_id(trader), symbol(symbol_), price(price_),
    quantity(qty_), initial_quantity(qty_), timestamp(std::chrono::steady_clock::now()) {}


} // namespace clob