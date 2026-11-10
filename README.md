# Agora

A proposed research society for market microstructure, electronic trading, and low-level systems.

[GitHub organization](https://github.com/agora-virtuale) · [Source repository](https://github.com/agora-virtuale/agora)

Agora's central project is a simulated electronic exchange: a C++ matching engine and limit order book, autonomous trading agents, reproducible experiments, and eventually a real-time infrastructure testbed.

**Status: core foundation implemented; exchange algorithms remain a scaffold.** Strong IDs and units, validated dates/times, separate simulation time, Greek-themed currencies, and checked cash arithmetic are working. Matching, book storage, scheduling, codecs, and trading strategies are intentionally left for you to implement. Unimplemented operations throw `std::logic_error`.

**Language mode:** C++26 when supported by the compiler/CMake pair, otherwise C++23. Configuration prints the selected mode and fails if neither is supported; it never silently downgrades to C++20. C++26 feature/library availability remains compiler-dependent ([GCC support status](https://gcc.gnu.org/projects/cxx-status.html#cxx26)).

## Start coding

Open this folder in **CLion** as a CMake project. Agora's interfaces are named modules in `modules/agora`; consumers use `import agora.core;` and the domain imports described in [Modules](docs/MODULES.md). On this Windows laptop, run this once from the repository root:

```powershell
powershell -NoProfile -File scripts/dev.ps1
```

The script discovers an installed MinGW GCC, CMake, and Ninja, creates ignored machine-local CMake presets, builds Debug, and runs the smoke check. In CLion select **local-debug** (reload CMake presets if necessary). No packages are downloaded and no global settings are changed.

On this laptop, CLion's **Debug-MinGW** profile and the **local-debug** preset use MSYS2 GCC 16.2 with CMake 4.4.5 in C++26 mode. MSVC remains a supported alternative using the C++23 fallback when necessary. The `agora_sim` run configuration builds and starts the scaffold status program, including its selected language mode. CLion's `cmake-build-*` output folders are ignored by Git.

```powershell
# Build, smoke-check, and show the starter executable:
powershell -NoProfile -File scripts/dev.ps1 -Run
# Release configuration:
powershell -NoProfile -File scripts/dev.ps1 -Configuration Release
```

With a module-capable compiler (GCC 14+ or a supported Clang/MSVC), Ninja 1.11+, and CMake 3.28+ already on PATH (Linux/macOS or a configured Windows developer shell):

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
./build/debug/agora_sim --help
```

MSVC is also supported: configure with `cmake -S . -B build/msvc`, then build with `cmake --build build/msvc --config Debug` and test with `ctest --test-dir build/msvc -C Debug --output-on-failure` from a Visual Studio developer shell. Don't reuse a build directory across toolchains.

## First implementation task

Read [the roadmap](docs/ROADMAP.md), then write tests for one resting order and implement `src/exchange/limit_order_book.cpp`. Extend the proposed interfaces as the tests require. The first milestone is one symbol with limit orders, price-time priority, partial fills, and cancellation.

## Repository map

| Path | Responsibility |
| --- | --- |
| `modules/agora/core.cppm` | Strong types, time, money, metadata, commands, and events |
| `modules/agora/exchange.cppm`, `src/exchange` | Order book and matching interfaces/implementations |
| `modules/agora/simulation.cppm`, `src/simulation` | Virtual time and event scheduling interfaces/implementations |
| `modules/agora/agents.cppm` | Strategy interface and participant-owned state |
| `modules/agora/protocols.cppm`, `src/protocols` | Future byte encoding, isolated from exchange logic |
| `apps/sim` | Simulation entry point; currently a scaffold status/usage program |
| `tests` | Functional test plan; add tests alongside each implemented feature |
| `benchmarks` | Performance experiments after correctness |
| `research` | Experiment designs, future Python analysis, and results guidance |
| `data` | Local input data; ignored except its README |
| `docs` | Architecture, requirements, decisions, project context, and roadmap |
| `scripts` | Local developer setup/build helpers |

Read [the core foundation](docs/FOUNDATION.md), [architecture](docs/ARCHITECTURE.md), [initial requirements](docs/REQUIREMENTS.md), [project context](docs/PROJECT_CONTEXT.md), and [contributing](CONTRIBUTING.md). Run `agora_sim --foundation` for the Greek-currency/time example. CI builds and tests on Windows and Linux and checks for undefined behavior on Linux. Functional core tests cover type separation, calendar/time boundaries, exact money arithmetic, and currency mismatches; exchange behavior is not implemented or tested yet.

## License

[MIT](LICENSE), copyright 2026 Agora contributors.
