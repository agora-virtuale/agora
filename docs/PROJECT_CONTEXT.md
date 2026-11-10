# Agora project context

This document captures the earlier Agora discussion and the setup choices for this repository. Ideas below are proposed plans, not claims of implemented features or measured performance.

## Vision

Agora is a proposed HFT and low-level systems research society. Its central engineering project is a simulated electronic exchange with a limit order book, matching engine, autonomous agents with private strategies, and research into the resulting market. Contributors can work on C++ infrastructure, trading algorithms, performance, and market microstructure.

The longer-term ideas include OUCH/ITCH-inspired interfaces, circular buffers, reproducible market experiments, and a real-time infrastructure testbed. None of those labels imply protocol compatibility, production readiness, or existing research results.

## Compute and hardware discussion

Start development and benchmark the first simulation on the existing laptop. An event-driven design can represent many agents as objects in one process; every agent does not need an OS process, thread, virtual machine, or LLM. Agent count by itself does not predict capacity: decision complexity, event volume, memory, and logging matter.

Simulation time is independent from wall time. Processing one simulated second may take more or less than one real second. Parallelizing independent runs is a later scaling option. CPU, RAM, and storage are the initial concerns; GPU work becomes relevant if a defined learning or batch-compute workload needs it.

Earlier discussion floated thousands of lightweight agents, a later dedicated workstation, and eventual shared research compute. Treat these as experiments/possible upgrades, not benchmarks or purchase commitments. Profile a working workload before buying hardware. No hardware procurement is part of this setup.

## Organization, repository, website, and recruitment

The GitHub organization is [agora-virtuale](https://github.com/agora-virtuale), owned by the project owner's personal account. The first-choice `agora` handle was already taken. The [Agora repository](https://github.com/agora-virtuale/agora) is public and MIT-licensed.

Use one initial repository with exchange, agents, simulation, protocols, research, and docs. Split it only when contributors or release boundaries justify that. Secure the identity early. A website should explain the mission, actual capabilities, contributor tasks, and research direction after a working technical foundation exists. Recruit around concrete engineering tasks and evidence, with membership/governance still to be defined.

No domain purchase, paid plan, institution affiliation, member invitation, or website publication is included in this repository setup.

## Setup scope agreed for this scaffold

Provide architecture, documentation, CMake, and compiling skeletons. The project owner implements the exchange. Stub operations fail explicitly; they do not simulate successful trades. Use CLion/CMake and the existing local compiler rather than install a new toolchain. These choices are a starting point and can be revised as the project develops.
