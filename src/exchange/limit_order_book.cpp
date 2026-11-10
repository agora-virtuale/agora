module;
#include <stdexcept>

module agora.exchange;

namespace agora::exchange {
LimitOrderBook::LimitOrderBook(SymbolId symbol) : symbol_(symbol) {}

void LimitOrderBook::add_resting(const NewOrder&) {
    throw std::logic_error("TODO: implement LimitOrderBook::add_resting");
}

Quantity LimitOrderBook::cancel(const CancelOrder&) {
    throw std::logic_error("TODO: implement LimitOrderBook::cancel");
}
} // namespace agora::exchange
