# Initial design decisions

| Decision | Reason | Revisit when |
| --- | --- | --- |
| One repository | Keep interfaces and examples in sync with minimal setup | Modules have independent release/ownership needs |
| C++26 preferred, C++23 fallback; CMake and standard library | Use the newest supported language mode while keeping the installed MSVC toolchain usable | Compiler/library support changes or a feature requires a specific toolchain |
| Integer ticks and lots | Explicit units and predictable arithmetic | Instrument metadata and fractional lots are specified |
| Single-threaded matching core first | Make ordering and correctness understandable | A correct measured baseline exposes a bottleneck |
| Virtual-time simulation first | Reproducible experiments without real-time throughput requirements | A defined real-time test requires an outer adapter |
| Agent objects and private strategy state | Scale without one process per agent | Isolation or a different runtime becomes a requirement |
| Fail-fast TODO operations | Make missing behavior visible | Each operation has tests and an implementation |
| Public MIT repository | Requested project distribution model | Owner explicitly changes licensing policy |
| Strong IDs and unit types | Catch identity/unit mix-ups during compilation | A measured need requires a specific boundary representation |
| Separate UTC and virtual timelines | Preserve deterministic simulation without mixing calendar and elapsed instants | A real-clock adapter is introduced |
| Greek identity; Athens first | Keep a focused starting venue while leaving room for additional cities | A contributor implements another venue and its routing/session rules |
| Attic/Aeginetic simulation units, integer cash atoms | Preserve drachma/obol denomination relationships and avoid floating-point cash | Explicit instrument, rounding, fee, or FX rules are introduced |
| Named modules for Agora interfaces | Explicit public module boundaries, as requested; compiler headers remain toolchain resources | Toolchain constraints or measured build costs justify revisiting the layout |

These are provisional engineering defaults. No choice of specific price-level container, event persistence format, RNG algorithm, protocol version, or concurrency model has been locked in.
