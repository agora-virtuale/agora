# Core foundation

The first implementation layer supplies domain values, civil dates, UTC timestamps, virtual time, currencies, and checked cash arithmetic. It has no external dependencies. Matching, scheduling, accounts, FX, and strategies remain future work.

All core APIs are exported by `agora.core` from `modules/agora/core.cppm`. Use `import agora.core;` in consumers; include any standard-library headers whose names the consumer uses. [Module/build guide](MODULES.md).

## Strong values and identifiers

`StrongValue<Tag, Rep>` stores one integer and provides explicit construction, `.value()` extraction, comparison, and hashing. Each alias has a different tag, so an `OrderId` cannot be passed where a `ParticipantId` is expected. Arithmetic is intentionally not defined between arbitrary strong values.

```cpp
agora::OrderId order{7};
agora::ParticipantId trader{7};
// order = trader; // Compile error, even though the stored integers match.
auto raw_order_id = order.value(); // Explicit extraction at a boundary.
```

| Type | Representation | Meaning |
| --- | --- | --- |
| `OrderId`, `ParticipantId`, `TradeId`, `Sequence` | `uint64_t` | Distinct identities/order sequence |
| `SymbolId`, `VenueId` | `uint32_t` | Instrument and venue identities |
| `Price` | `int64_t` | Price ticks per whole lot |
| `Quantity` | `uint64_t` | Number of whole lots |
| `CashAtoms` | `int64_t` | Smallest simulation cash unit, qualified by `Money`'s currency |

Zero IDs and sequences mean unspecified; the future command validator must reject them for operations requiring an identity. Strong types prevent accidental mixing; they do not validate every business rule. `Price{0}` and `Quantity{0}` can be represented, while `checked_notional` rejects them. Extracting an integer is a deliberate escape hatch, so preserve the domain type through application code.

`MarketId{venue, symbol}` identifies an instrument at a venue. Keep a shared `SymbolId` for the same instrument when adding venues later. There is no cross-venue routing implementation yet.

## Dates and UTC civil timestamps

- `Date` is `std::chrono::year_month_day`, using the proleptic Gregorian calendar, including astronomical year zero and negative years. This does not reproduce ancient Greek calendars.
- `make_date` checks raw year/month/day bounds before constructing chrono's compact fields and rejects impossible dates, including invalid leap days.
- `TimeOfDay` is a validated nanosecond offset within `[00:00:00, 24:00:00)`. `from_hms` rejects hour 24, second 60, and out-of-range fractions. Default construction means midnight.
- `UtcTimestamp` is a system-clock/Unix timeline with signed 64-bit nanoseconds. It represents UTC civil labels without leap seconds; it is not `std::chrono::utc_clock`.
- `make_utc_timestamp` rejects valid calendar dates outside the nanosecond timeline's range (approximately 1677–2262). `split_utc_timestamp` round-trips every representable timestamp, including negative epochs and the signed integer endpoints.

```cpp
auto date = agora::make_date(2026, 11, 9);
auto time = agora::TimeOfDay::from_hms(14, 30, 0, 123);
if (date && time) {
    auto timestamp = agora::make_utc_timestamp(*date, *time);
    if (timestamp) {
        auto epoch_nanoseconds = agora::unix_ns(*timestamp);
        auto civil = agora::split_utc_timestamp(*timestamp);
        // Store/consume epoch_nanoseconds or civil here.
    }
}
```

No date-string parser, time-zone conversion, DST handling, market-session calendar, or wall-clock reads are included. The owning application will choose any real-clock access; matching logic stays independent of it.

## Simulation time

`SimTime` is a distinct non-negative elapsed instant, with origin zero and nanosecond resolution. It cannot convert implicitly to or from a UTC timestamp. The simulator owns its current instant; there is no global `now()` or global mutable clock.

`SimTime::from_elapsed` rejects negative time. `advance_sim_time` rejects negative increments and overflow. `elapsed_between` rejects time regression. All return `std::expected` for invalid operations. The largest virtual instant is `INT64_MAX` nanoseconds after origin.

## Greek-themed currencies

Athens defaults to the **Attic drachma**. A separate **Aeginetic drachma** definition provides another Greek-standard-inspired unit without adding a second venue.

| Internal code | Simulation unit | Atoms per obol | Obols per drachma |
| --- | --- | --- | --- |
| `ATT` | Attic drachma | 1,000,000 | 6 |
| `AEG` | Aeginetic drachma | 1,000,000 | 6 |

The denomination factors follow the documented relationship **1 drachma = 6 obols = 12 hemiobols**. A tetradrachm is four drachmas. Ancient standards differed in weight; the project makes no claim of parity between them. [Historical reference: Banca d'Italia, Minted coins, display case 5](https://www.bancaditalia.it/servizi-cittadino/musei-e-visite/museo-moneta/moneta-coniata/index.html?com.dotmarketing.htmlpage.language=1).

One million computational atoms per obol is **Agora's simulation precision**, not a historical denomination. `ATT` and `AEG` are internal codes, not ISO currency codes. No silver weight, real-money value, or historical/modern FX rate is encoded. Obols and drachmas are denominations within a currency, not separate FX currencies.

`Money` qualifies signed cash atoms with a validated currency. Negative amounts are allowed for cash changes and balances; default construction is zero `ATT`. `checked_add` and `checked_subtract` reject different currencies even for zero amounts. `checked_scale` and denomination construction reject signed overflow. There is no implicit monetary ordering or conversion between currencies.

## Prices, lots, and tick value

`Price` is a count of ticks, while `Money` is a currency amount. To turn a quoted order into a cash notional, provide the cash value of **one price tick for one whole lot**:

```cpp
auto tick = agora::Money::from_atoms(agora::Currency::attic_drachma,
                                    agora::CashAtoms{1'000});
if (tick) {
    auto notional = agora::checked_notional(agora::Price{125}, agora::Quantity{4}, *tick);
    // Result: 500,000 ATT atoms, or half an ATT obol.
    // Check notional before accessing its result.
}
```

The calculation is `price_ticks × whole_lots × tick_value_atoms`. Price, lot count, and tick value must all be positive. Multiplication is checked before it happens, including quantities larger than signed 64-bit range. No floating-point calculation, rounding, fee model, account ledger, or lot-size conversion is performed. The future instrument catalogue will supply each market's tick value and lot definition.

## Venue metadata and future cities

`athens` provides venue ID 1, code `ATH`, name `Agora Athens`, IANA-style zone identifier `Europe/Athens`, and default quote currency `ATT`. Its strings have static lifetime. The zone identifier is metadata, not a fixed UTC offset or a working conversion service.

A future contributor can add venue definitions for other cities and route the same `SymbolId` to different `MarketId`s. Sessions, visibility, latency, transport, FX, bot behavior, and arbitrage rules will need explicit implementations and tests. Keep the Greek project identity independent of those venue configurations.

## Try it

Run `agora_sim --foundation` to see a deterministic example of Athens, exact drachma/obol units, notional calculation, the Unix epoch, and a virtual-time step. `core_foundation` tests compile-time separation and runtime calendar, overflow, currency, and time-boundary behavior. Test failures remain active in Release builds.
