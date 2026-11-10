# Implementation roadmap

## 0. Ready workspace — scaffold provided

- C++26-preferred targets with a C++23 fallback, named-module interfaces, implementation units, CLI status executable, CMake presets.
- Windows tool-discovery/build script, MIT license, coding guide, CI definition.
- Documentation of the proposed design and research direction.
- Working core foundation: strong IDs/units, validated calendar/time types, separate UTC and simulation time, exact Greek-themed currency arithmetic, and Athens venue metadata.
- Functional core boundary tests, a CLI foundation demo, and build/runtime smoke checks. Exchange behavior remains unimplemented.

## 1. Resting order book — start here

1. Define positive tick/lot bounds and active-order ID uniqueness.
2. Add behavioral tests for an empty book, one order, several price levels, and FIFO at one price.
3. Choose storage for levels, queues, and the ID index; implement `add_resting`.
4. Implement cancellation; test head/middle/tail cancellation, empty-level removal, and invalid ownership.

Done when the index, queues, and level quantities stay consistent across all tests. Add the inspection API your tests need instead of exposing mutable internals.

## 2. Matching engine

Define event order and maker-price execution. Implement non-crossing orders, crossing orders, partial fills, multi-level sweeps, and deterministic rejection. Test quantity conservation, priority, invalid commands, and numeric boundaries. Keep it single-threaded.

## 3. Deterministic simulation

Implement the scheduler and controller, sequence assignment, a virtual clock, reproducible RNG streams, and scripted agent callbacks. Record ordered commands/events and a run manifest. Add a replay test before adding randomness. Route public and participant-specific events separately.

## 4. Research agents and analytics

Add simple liquidity providers, liquidity takers, and exploratory strategies. Implement positions/cash before reporting P&L. Specify a versioned event-log schema, experiment configurations, and Python analysis. Compare scenarios across multiple seeds; distinguish an observed result from a hypothesis.

## 5. Measure and optimize

Establish correctness-preserving benchmarks. Measure event throughput, latency distribution, memory, allocations, and log overhead. Only then consider circular buffers, cache layout, preallocation, and alternate book structures. Keep reproducibility checks during optimization.

## 6. Protocols and real-time testbed

Specify and test framing/codecs; add malformed-input tests. Add transport and real-time clock adapters outside the exchange. Simulated latency and actual networking are distinct experimental modes.

## 7. Public project and recruitment

Reserve the organization identity, publish a focused repository, and create contributor-sized tasks. Launch a simple website once a working exchange, documented examples, and measured results can be shown. Establish governance, membership expectations, and research review practices before broad recruitment. Dates remain flexible.
