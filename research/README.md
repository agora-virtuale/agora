# Research workspace

Keep experiment designs and reusable analysis here. Future Python code should consume recorded exchange events rather than call into the matching hot path. Create a Python environment and dependency file only when an actual analysis requires them.

For each experiment, record the question, hypothesis, exchange rules, population and strategy parameters, input data identity, seed(s), simulation horizon, commit, compiler, metrics, and limitations. Report spread/depth/volume and position/cash/P&L only after defining their measurement and accounting rules.

Possible directions: liquidity under different agent mixes, the effect of latency on execution, price formation, and throughput/correctness tradeoffs. Simulated outcomes are properties of the declared model; comparison to a real market needs separate data and validation.
