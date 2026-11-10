# Benchmarks

Add benchmarks after a behaviorally tested exchange exists. Keep correctness tests separate from performance measurement.

Record compiler/version/flags, build mode, machine, dataset or generated workload, seed, repetitions, warmup, timer method, throughput, latency distribution, allocations, and peak memory. Measure logging overhead separately. Store experiment metadata with results under `research/results` or an external artifact store.

No throughput or agent-capacity claims have been measured by this scaffold. Circular buffers, custom allocators, and lock-free structures are later experiments.
