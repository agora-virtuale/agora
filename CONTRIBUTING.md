# Contributing

Start with a scoped issue describing observable behavior and its acceptance criteria. Use short branches such as `feature/resting-order-book` and small pull requests. Keep the main branch buildable.

Follow the [code of conduct](CODE_OF_CONDUCT.md). Report vulnerabilities through the [security policy](SECURITY.md), and accessibility barriers through the process in [Accessibility](ACCESSIBILITY.md).

## Engineering rules

- Keep commit messages very short and no longer than one sentence unless explicitly requested.
- Prefer C++26; support C++23 as the fallback for toolchains without C++26 mode. Check feature/library availability before using a C++26-only facility. Standard library first, four-space indentation, `snake_case` files/functions, `PascalCase` types. `.clang-format` records formatting preferences.
- Keep public named-module interfaces under `modules/agora` as `.cppm` files, implementation units under `src`, and entry points under `apps`. Register interfaces with the target's public `FILE_SET CXX_MODULES`. Consumers import Agora modules; standard-library includes belong before the module declaration in a global module fragment. See `docs/MODULES.md`.
- Use integer price ticks, whole-lot quantities, and explicit virtual timestamps. Define bounds and check arithmetic before using external inputs.
- No wall-clock reads, sleeps, network calls, global RNG, or thread scheduling dependencies inside matching logic.
- Add behavior tests as features are implemented. A scaffold smoke check does not establish trading correctness.
- Keep rejection paths explicit. Do not replace TODO exceptions with apparent success or silently dropped messages.
- Record a seed, configuration, commit, and input dataset identity for each research run. Strategies own their private state; agents receive only their permitted observations.
- Measure before optimizing. Record hardware, compiler flags, workload, sample counts, and latency distribution with benchmarks.
- Keep credentials, proprietary market data, personal information, build output, and local IDE settings out of Git.

Before a pull request, build and run CTest in Debug and Release. On this Windows setup, use `scripts/dev.ps1` and `scripts/dev.ps1 -Configuration Release`. The configured CI will also build with GCC on Linux and MSVC on Windows once pushed.

Contributions are supplied under the repository's MIT license. Project governance and membership rules are still to be decided; no institutional affiliation is claimed.
