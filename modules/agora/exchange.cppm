module;
#include <vector>

export module agora.exchange;
export import agora.core;

export namespace agora::exchange {
// Implementation seam only. Choose storage/indexes after writing book invariants.
class LimitOrderBook {
public:
    explicit LimitOrderBook(SymbolId symbol);
    void add_resting(const NewOrder& order);
    Quantity cancel(const CancelOrder& order);
    [[nodiscard]] SymbolId symbol() const noexcept { return symbol_; }

private:
    SymbolId symbol_;
    // TODO: price levels, FIFO queues, and order-id index.
};
} // namespace agora::exchange

export namespace agora::exchange {
class MatchingEngine {
public:
    explicit MatchingEngine(SymbolId symbol);
    [[nodiscard]] std::vector<MarketEvent> process(const Command& command);

private:
    LimitOrderBook book_;
    // TODO: deterministic validation, matching, and event ordering.
};
} // namespace agora::exchange
