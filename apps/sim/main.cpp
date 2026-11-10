#include <iostream>
#include <string_view>

import agora.core;
import agora.exchange;
import agora.simulation;
import agora.agents;
import agora.protocols;

namespace {
void print_foundation() {
    using namespace agora;
    constexpr auto drachma = *Money::from_denomination(
        Currency::attic_drachma, Denomination::drachma, 1);
    constexpr auto cost = *checked_notional(Price{125}, Quantity{4},
        *Money::from_atoms(Currency::attic_drachma, CashAtoms{1'000}));
    constexpr auto epoch = *make_utc_timestamp(*make_date(1970, 1, 1));
    constexpr auto step = *advance_sim_time(SimTime{}, Nanoseconds{1'000});
    std::cout << "Venue: " << athens.name << " (" << athens.time_zone << " metadata)\n"
              << "ATT: 6 obols = 1 drachma = " << drachma.atoms().value() << " atoms\n"
              << "Example notional: 125 ticks x 4 lots x 1000 atoms/tick/lot = "
              << cost.atoms().value() << " ATT atoms\n"
              << "Unix epoch (1970-01-01T00:00:00Z): " << unix_ns(epoch) << " ns\n"
              << "Simulation step from origin: " << step.elapsed().count() << " ns\n";
}
} // namespace

int main(int argc, char** argv) {
    if (argc > 2 || (argc == 2 && std::string_view{argv[1]} != "--help" &&
                     std::string_view{argv[1]} != "--version" &&
                     std::string_view{argv[1]} != "--foundation")) {
        std::cerr << "Usage: agora_sim [--help | --version | --foundation]\n";
        return 2;
    }
    std::cout << "Agora 0.1.0\n";
    if (argc == 2 && std::string_view{argv[1]} == "--version") {
        return 0;
    }
    if (argc == 2 && std::string_view{argv[1]} == "--foundation") {
        print_foundation();
        return 0;
    }
    std::cout << "C++" << AGORA_CXX_STANDARD << " market simulation research scaffold\n"
                 "Exchange, scheduler, and protocol algorithms await implementation.\n"
                 "Core types, time, and Greek currency demo: --foundation\n"
                 "Start: docs/ROADMAP.md and src/exchange/limit_order_book.cpp\n"
                 "Usage: agora_sim [--help | --version | --foundation]\n";
    return 0;
}
