# Named modules

Agora's public C++ interfaces are named modules. Domain values, APIs, and behavior are unchanged by the migration; consumers import a module instead of including an Agora header.

| Import | Interface unit | Responsibility |
| --- | --- | --- |
| `agora.core` | `modules/agora/core.cppm` | Strong values, dates/time, currencies, money, venue metadata, commands/events |
| `agora.exchange` | `modules/agora/exchange.cppm` | Order book and matching engine interfaces |
| `agora.simulation` | `modules/agora/simulation.cppm` | Event scheduler interface |
| `agora.agents` | `modules/agora/agents.cppm` | Trading agent interface |
| `agora.protocols` | `modules/agora/protocols.cppm` | Future byte codec interface |

Each domain module re-exports `agora.core`. The application imports the domain modules it uses; core tests import `agora.core`. There are no Agora-owned `.hpp` interfaces or header compatibility shims.

```cpp
#include <iostream>

import agora.core;

int main() {
    auto cash = agora::Money::from_denomination(
        agora::Currency::attic_drachma, agora::Denomination::obol, 6);
    if (cash) {
        std::cout << cash->atoms().value() << '\n';
    }
}
```

## Build configuration

Use CMake 3.28+ with a supported compiler/generator. The local configuration is GCC 16.2, CMake 4.4.5, and Ninja 1.13.2. GCC must be at least version 14 for CMake's dependency scanner; the configured CI uses GCC 15 on Ubuntu 26.04 and MSVC on Windows. Keep the C++26-preferred/C++23-fallback language policy.

The Linux UBSan job uses Clang 22. GCC 15 passed the ordinary build but crashed internally while writing the core module with UBSan instrumentation; changing sanitizer compilers preserves coverage without disabling checks.

The CMake targets register interface units with a public `FILE_SET CXX_MODULES`. CMake scans dependencies, orders module compilation, and supplies the compiler's module maps. Do not manually build or distribute BMI/CMI files, and do not mix build directories between compilers or configurations.

Ninja 1.11+ supports scanning, as do supported Visual Studio generators. Linux examples/presets use Ninja; the Windows CI uses its installed Visual Studio generator. Makefile generators are not supported for this project’s module build.

After updating an existing checkout, reload the CMake project in CLion and build normally. The active local profile remains `Debug-MinGW`. `.cppm` keeps the interfaces discoverable by CLion's module support.

## Standard and platform headers

The standard library is still included through ordinary headers. A module unit puts those includes in its **global module fragment**, before `export module ...` or `module ...`:

```cpp
module;
#include <vector>

export module agora.exchange;
export import agora.core;

export namespace agora::exchange {
    // Public interface declarations.
}
```

Implementation units begin with a global module fragment for standard headers they directly need, then `module agora.exchange;` (or their corresponding module). Importers include the standard headers whose names they use; standard declarations do not become a general-purpose `std` namespace export from Agora.

This migration does not use header units or `import std`. CMake's `import std` support remains experimental and has additional toolchain/generator restrictions. Toolchain/system headers, including MinGW's Windows headers, remain installed and visible in CLion's External Libraries. They are not Agora source or checked into this repository.

## Compile-time measurements

Named modules can avoid repeated interface parsing, but they also add dependency-scanning work and an interface compilation step before importers can compile. Performance must be measured on a particular workload.

An initial comparison used the same laptop, GCC 16.2, CMake 4.4.5, Ninja, Debug configuration, tests enabled, and four parallel jobs. Each result below is the median of three builds after a warmup. A clean build removes compiler outputs while retaining the warm filesystem cache. The app-edit case touches `apps/sim/main.cpp` without changing its contents and rebuilds.

| Workload | Headers (commit `5a19498`) | Named modules prototype |
| --- | --- | --- |
| Clean build | 2.72 s | 4.95 s |
| App source edit/rebuild | 1.26 s | 1.51 s |

At this small project size the module version was slower in both measured cases. These timings are illustrative local measurements, not a portable benchmark or a promise about a larger future exchange. The migration adopts explicit module interfaces; it does not claim a compile-time speedup. Re-measure as the source graph grows.

## References

- [CMake 4.4 module support and scanning](https://cmake.org/cmake/help/v4.4/manual/cmake-cxxmodules.7.html)
- [GCC module implementation and limitations](https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Modules.html)
- [CLion module support](https://www.jetbrains.com/help/clion/support-for-c-20-modules.html)
