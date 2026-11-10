# First exchange milestone

These are implementation acceptance criteria, not current capabilities.

## Scope

- One instrument per book; positive price and quantity, with defined maximum values.
- Limit orders, good till cancelled. Add market orders and other time-in-force policies only after defining their rules.
- Unique active order IDs; define whether reuse after completion is allowed.
- Better price first, FIFO within a price level. Specify maker-price execution before implementing crossing.
- Partial fills update remaining quantity without changing the resting order's queue priority.
- Cancel validates participant ownership and symbol, handles unknown/filled IDs explicitly, and removes empty levels.
- Invalid commands leave book state unchanged and return a defined rejection event.
- Define event ordering for acceptance, each fill, residual resting quantity, and cancellation. The draft event types may need extension.

## Invariants to test

- No negative/overflowed quantities; no zero-sized resting orders or empty price levels.
- Quantity conservation for every fill. Both sides reference the same execution price and size.
- Every resting order appears exactly once and has a consistent index entry.
- After matching an incoming order, no eligible crossed prices remain.
- Equal-price fills follow arrival order, including after partial fills and cancellations.
- Given the same ordered command stream, the exchange emits the same event stream.
- Scheduler time never regresses; equal-time commands follow the declared sequence rule.

## Open decisions before expanding scope

Specify cancel acknowledgements/rejections, order-ID reuse, self-trade prevention, replace priority, tick/lot metadata, timestamp attribution, trade IDs, private execution reports versus public market data, and latency routing. The current shared `MarketEvent` variant is an interface sketch; the controller must not broadcast private messages to every agent.

Choose and document these rules as you implement each feature. The initial scaffold intentionally does not encode answers to every market-design question.
