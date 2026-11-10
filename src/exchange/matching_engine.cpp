module;
#include <stdexcept>
#include <vector>

module agora.exchange;

namespace agora::exchange {
MatchingEngine::MatchingEngine(SymbolId symbol) : book_(symbol) {}

std::vector<MarketEvent> MatchingEngine::process(const Command&) {
    throw std::logic_error("TODO: implement MatchingEngine::process");
}
} // namespace agora::exchange
