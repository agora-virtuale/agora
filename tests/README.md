# Tests

The current CTests are:

- `core_foundation`: compile-time type separation and runtime ID/hash, calendar, UTC/simulation time, currency, overflow, and quote-notional checks.
- `scaffold_smoke`: the executable's version command.
- `foundation_demo`: the deterministic Athens/currency/time CLI example.

Core failures use explicit checks that remain enabled in Release. Windows and Linux CI run the tests; Linux also runs an undefined-behavior sanitizer build. None of these tests establishes exchange correctness.

As you implement the exchange, add a separate C++ test executable here and register it with `add_test` in the root CMake file. Prefer explicit failure checks or a test framework; do not rely on `assert` alone, since Release may disable assertions.

Start with resting-order insertion, level ordering, FIFO, duplicate IDs, cancellation, and consistent index/queue state. Then cover crossing, partial fills, multi-level sweeps, owner/symbol validation, numeric limits, and quantity conservation. Follow [requirements](../docs/REQUIREMENTS.md).

Later add deterministic scheduler ties, replay equality, log round trips, accounting conservation, and malformed protocol input. Add a third-party testing dependency only through an explicit dependency decision; none is downloaded by the scaffold.
