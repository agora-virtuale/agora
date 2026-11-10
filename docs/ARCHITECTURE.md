# Architecture

The core foundation (domain types, dates/times, currencies, and checked cash arithmetic) is implemented. The exchange/simulation design below is proposed; trading algorithms and data structures are not implemented.

## Data flow

```text
Agent private state -> Command -> Event scheduler (time, sequence)
                                        |
                                        v
                              Matching engine -> Order book
                                        |
                                        v
                              Ordered market events
                               /                 \
                    Visibility/latency          Event log
                          layer                    |
                            |                Research analysis
                            v
                           Agent
```

The visibility/latency layer, controller, event logging, and research pipeline are future components. They are described here to preserve clean boundaries, not represented as working code.

## Modules and dependency direction

`core` has no dependencies. `exchange`, `simulation`, `agents`, and `protocols` depend on `core`. The application composes modules. Protocol encoders know domain messages; exchange logic never depends on byte layouts, sockets, research notebooks, or an agent implementation.

These boundaries are implemented as named C++ modules: `agora.core`, `agora.exchange`, `agora.simulation`, `agora.agents`, and `agora.protocols`. Interfaces live in `modules/agora/*.cppm`; CMake manages compiled interfaces and dependency scanning. Standard/platform headers remain toolchain dependencies. See [module contracts and measurements](MODULES.md).

- **Exchange:** one symbol per initial book. Validation and price-time matching produce events. Use an order-id index for cancellation; choose level/queue storage after correctness tests. Ownership checks belong in command processing.
- **Simulation:** a virtual clock and a queue ordered by `(time, sequence)`. Define unique sequence assignment and equal-time ordering before coding. Keep simulation time independent from execution speed.
- **Agents:** lightweight objects in one process, with participant-owned state and seeded random streams. Start with simple scripted agents. A separate process/thread/LLM per agent is not required.
- **Protocols:** reserve a boundary for a documented internal wire format inspired by order-entry and market-data protocols. Do not serialize C++ structs directly. Define endian order, widths, framing, validation, and versioning first. OUCH/ITCH compatibility is not implemented or promised.
- **Infrastructure (later):** add circular buffers, transport adapters, logging, and allocation policies only when a measured workload calls for them. The scaffold has no concurrent queue implementation.
- **Research (later):** consume event logs without entering the matching path. Python notebooks/scripts can compute market statistics, positions, cash, and P&L after accounting rules are defined.

## Two execution modes, one exchange core

The first mode is an accelerated event-driven research simulation. A later real-time testbed may use sockets and monotonic wall-clock scheduling around the same exchange core. Sharing the core is a direction, not a requirement to implement both modes now. Simulation latency can be represented by future timestamps without actual network traffic or sleeping.

## Initial scope

Prefer C++26 and the standard library with CMake. If the compiler/CMake pair does not support C++26 mode, require C++23 instead. The selected mode is printed at configure time and by the starter executable. Feature/library support still varies by compiler; the fallback does not emulate C++26-only facilities. One repository keeps interface changes atomic. Avoid distributed infrastructure, lock-free structures, custom allocators, and protocol-compliance claims until there is a tested baseline.

Prices are strongly typed signed integer ticks per whole lot, quantities are strongly typed unsigned lot counts, and simulation time is a separate non-negative instant with signed 64-bit nanosecond storage. UTC civil timestamps use the Unix/system-clock timeline and are distinct from virtual time. IDs have distinct tags; zero is an unspecified sentinel awaiting future command validation. Greek-themed currencies use checked signed cash atoms, with explicit tick value for notional calculations. Athens is the first venue; additional city venues are a future contributor task. See [core contracts and limitations](FOUNDATION.md). There is no current risk engine, account ledger, production transport, or live-market connection.
