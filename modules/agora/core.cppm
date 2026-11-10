module;
#include <chrono>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <functional>
#include <limits>
#include <optional>
#include <ratio>
#include <string>
#include <string_view>
#include <variant>

export module agora.core;

export namespace agora {
// A domain label with explicit construction/extraction and no implicit arithmetic.
// Range and business-rule validation belongs to the operation using the value.
template <class Tag, std::integral Rep>
class StrongValue {
public:
    using value_type = Rep;

    constexpr StrongValue() noexcept = default;
    explicit constexpr StrongValue(Rep value) noexcept : value_(value) {}

    [[nodiscard]] constexpr Rep value() const noexcept { return value_; }
    constexpr auto operator<=>(const StrongValue&) const noexcept = default;

private:
    Rep value_{};
};
} // namespace agora

// Specializations are reachable through the imported interface; they cannot be
// explicitly exported.
template <class Tag, std::integral Rep>
struct std::hash<agora::StrongValue<Tag, Rep>> {
    [[nodiscard]] std::size_t operator()(agora::StrongValue<Tag, Rep> value) const noexcept {
        return std::hash<Rep>{}(value.value());
    }
};

// Calendar dates, UTC timestamps, and virtual time.
export namespace agora {
using Nanoseconds = std::chrono::duration<std::int64_t, std::nano>;
using Date = std::chrono::year_month_day;
// UTC civil time on the Unix/system-clock timeline; leap seconds are not represented.
using UtcTimestamp = std::chrono::sys_time<Nanoseconds>;

enum class TimeError {
    invalid_date,
    invalid_time_of_day,
    timestamp_out_of_range,
    negative_duration,
    overflow,
    time_regression
};

[[nodiscard]] constexpr std::expected<Date, TimeError> make_date(int year, unsigned month,
                                                               unsigned day) noexcept {
    // Check before constructing chrono fields: their small representations may wrap.
    if (year < static_cast<int>(std::chrono::year::min()) ||
        year > static_cast<int>(std::chrono::year::max()) ||
        month < 1 || month > 12 || day < 1 || day > 31) {
        return std::unexpected(TimeError::invalid_date);
    }
    const Date date{std::chrono::year{year}, std::chrono::month{month}, std::chrono::day{day}};
    if (!date.ok()) {
        return std::unexpected(TimeError::invalid_date);
    }
    return date;
}

class TimeOfDay {
public:
    constexpr TimeOfDay() noexcept = default; // Midnight.

    [[nodiscard]] static constexpr std::expected<TimeOfDay, TimeError>
    from_elapsed(Nanoseconds elapsed) noexcept {
        if (elapsed < Nanoseconds::zero() || elapsed >= std::chrono::days{1}) {
            return std::unexpected(TimeError::invalid_time_of_day);
        }
        return TimeOfDay{elapsed};
    }

    [[nodiscard]] static constexpr std::expected<TimeOfDay, TimeError>
    from_hms(unsigned hour, unsigned minute, unsigned second, unsigned nanosecond = 0) noexcept {
        if (hour > 23 || minute > 59 || second > 59 || nanosecond >= 1'000'000'000) {
            return std::unexpected(TimeError::invalid_time_of_day);
        }
        return TimeOfDay{std::chrono::hours{hour} + std::chrono::minutes{minute} +
                         std::chrono::seconds{second} + Nanoseconds{nanosecond}};
    }

    [[nodiscard]] constexpr Nanoseconds since_midnight() const noexcept { return elapsed_; }
    constexpr auto operator<=>(const TimeOfDay&) const noexcept = default;

private:
    explicit constexpr TimeOfDay(Nanoseconds elapsed) noexcept : elapsed_(elapsed) {}
    Nanoseconds elapsed_{};
};

[[nodiscard]] constexpr UtcTimestamp utc_from_unix_ns(std::int64_t count) noexcept {
    return UtcTimestamp{Nanoseconds{count}};
}

[[nodiscard]] constexpr std::int64_t unix_ns(UtcTimestamp timestamp) noexcept {
    return timestamp.time_since_epoch().count();
}

[[nodiscard]] constexpr std::expected<UtcTimestamp, TimeError>
make_utc_timestamp(Date date, TimeOfDay time = {}) noexcept {
    if (!date.ok()) {
        return std::unexpected(TimeError::invalid_date);
    }
    constexpr std::int64_t scale = 1'000'000'000;
    constexpr auto minimum = std::numeric_limits<std::int64_t>::min();
    constexpr auto maximum = std::numeric_limits<std::int64_t>::max();
    constexpr auto minimum_second = minimum / scale - 1;
    constexpr auto minimum_fraction = minimum % scale + scale;
    constexpr auto maximum_second = maximum / scale;
    constexpr auto maximum_fraction = maximum % scale;

    // Calendar years fit safely in whole seconds, even if they do not fit in nanoseconds.
    const auto day_count = std::chrono::sys_days{date}.time_since_epoch().count();
    const auto time_ns = time.since_midnight().count();
    const auto seconds = day_count * std::int64_t{86'400} + time_ns / scale;
    const auto fraction = time_ns % scale;
    if (seconds < minimum_second || seconds > maximum_second ||
        (seconds == minimum_second && fraction < minimum_fraction) ||
        (seconds == maximum_second && fraction > maximum_fraction)) {
        return std::unexpected(TimeError::timestamp_out_of_range);
    }
    if (seconds == minimum_second) {
        // Multiplying this one whole-second value by scale would underflow.
        return utc_from_unix_ns(minimum + (fraction - minimum_fraction));
    }
    return utc_from_unix_ns(seconds * scale + fraction);
}

struct UtcDateTime {
    Date date;
    TimeOfDay time;
    constexpr bool operator==(const UtcDateTime&) const noexcept = default;
};

[[nodiscard]] constexpr UtcDateTime split_utc_timestamp(UtcTimestamp timestamp) noexcept {
    constexpr std::int64_t day_ns = 86'400'000'000'000;
    const auto count = unix_ns(timestamp);
    auto days = count / day_ns;
    auto within_day = count % day_ns;
    if (within_day < 0) {
        --days;
        within_day += day_ns;
    }
    // Avoid converting the floor day itself to nanoseconds at the minimum timestamp.
    return {Date{std::chrono::sys_days{std::chrono::days{days}}},
            *TimeOfDay::from_elapsed(Nanoseconds{within_day})};
}

// An elapsed virtual instant, deliberately unrelated to system_clock or utc_clock.
// The scheduler owns the current instant; there is no global SimTime::now().
class SimTime {
public:
    constexpr SimTime() noexcept = default;

    [[nodiscard]] static constexpr std::expected<SimTime, TimeError>
    from_elapsed(Nanoseconds elapsed) noexcept {
        if (elapsed < Nanoseconds::zero()) {
            return std::unexpected(TimeError::negative_duration);
        }
        return SimTime{elapsed};
    }

    [[nodiscard]] constexpr Nanoseconds elapsed() const noexcept { return elapsed_; }
    constexpr auto operator<=>(const SimTime&) const noexcept = default;

private:
    explicit constexpr SimTime(Nanoseconds elapsed) noexcept : elapsed_(elapsed) {}
    Nanoseconds elapsed_{};
};

[[nodiscard]] constexpr std::expected<SimTime, TimeError>
advance_sim_time(SimTime now, Nanoseconds delta) noexcept {
    if (delta < Nanoseconds::zero()) {
        return std::unexpected(TimeError::negative_duration);
    }
    if (delta.count() > std::numeric_limits<std::int64_t>::max() - now.elapsed().count()) {
        return std::unexpected(TimeError::overflow);
    }
    return SimTime::from_elapsed(now.elapsed() + delta);
}

[[nodiscard]] constexpr std::expected<Nanoseconds, TimeError>
elapsed_between(SimTime begin, SimTime end) noexcept {
    if (end < begin) {
        return std::unexpected(TimeError::time_regression);
    }
    return end.elapsed() - begin.elapsed();
}
} // namespace agora

// Domain identifiers and units.
export namespace agora {
namespace detail {
struct OrderIdTag;
struct ParticipantIdTag;
struct SymbolIdTag;
struct VenueIdTag;
struct TradeIdTag;
struct SequenceTag;
struct PriceTag;
struct QuantityTag;
struct CashAtomsTag;
} // namespace detail

// Zero is the unspecified ID/sequence sentinel; command validation rejects it.
using OrderId = StrongValue<detail::OrderIdTag, std::uint64_t>;
using ParticipantId = StrongValue<detail::ParticipantIdTag, std::uint64_t>;
using SymbolId = StrongValue<detail::SymbolIdTag, std::uint32_t>;
using VenueId = StrongValue<detail::VenueIdTag, std::uint32_t>;
using TradeId = StrongValue<detail::TradeIdTag, std::uint64_t>;
using Sequence = StrongValue<detail::SequenceTag, std::uint64_t>;
using Price = StrongValue<detail::PriceTag, std::int64_t>;         // Integer ticks per lot.
using Quantity = StrongValue<detail::QuantityTag, std::uint64_t>; // Whole lots.
using CashAtoms = StrongValue<detail::CashAtomsTag, std::int64_t>;

// The same instrument may trade at multiple venues without changing its SymbolId.
struct MarketId {
    VenueId venue;
    SymbolId symbol;
    constexpr auto operator<=>(const MarketId&) const noexcept = default;
};

enum class Side { buy, sell };
} // namespace agora

// Greek currency definitions.
export namespace agora {
// Historical-standard-inspired simulation units. Codes are internal, not ISO 4217.
enum class Currency : std::uint8_t { attic_drachma = 1, aeginetic_drachma = 2 };
enum class Denomination : std::uint8_t { hemiobol, obol, drachma, tetradrachm };

struct CurrencyDefinition {
    Currency currency;
    std::string_view code;
    std::string_view name;
    std::int64_t atoms_per_obol;
    std::int64_t obols_per_drachma;
};

[[nodiscard]] constexpr std::optional<CurrencyDefinition>
currency_definition(Currency currency) noexcept {
    switch (currency) {
    case Currency::attic_drachma:
        return CurrencyDefinition{currency, "ATT", "Attic drachma", 1'000'000, 6};
    case Currency::aeginetic_drachma:
        return CurrencyDefinition{currency, "AEG", "Aeginetic drachma", 1'000'000, 6};
    }
    return std::nullopt;
}

[[nodiscard]] constexpr std::optional<std::int64_t>
denomination_atoms(Currency currency, Denomination denomination) noexcept {
    const auto definition = currency_definition(currency);
    if (!definition) {
        return std::nullopt;
    }
    const auto obol = definition->atoms_per_obol;
    switch (denomination) {
    case Denomination::hemiobol: return obol / 2;
    case Denomination::obol: return obol;
    case Denomination::drachma: return obol * definition->obols_per_drachma;
    case Denomination::tetradrachm: return obol * definition->obols_per_drachma * 4;
    }
    return std::nullopt;
}
} // namespace agora

// Internal checked integer arithmetic.
namespace agora::detail {
[[nodiscard]] constexpr std::optional<std::int64_t>
add_integer(std::int64_t a, std::int64_t b) noexcept {
    constexpr auto min = std::numeric_limits<std::int64_t>::min();
    constexpr auto max = std::numeric_limits<std::int64_t>::max();
    if ((b > 0 && a > max - b) || (b < 0 && a < min - b)) {
        return std::nullopt;
    }
    return a + b;
}

[[nodiscard]] constexpr std::optional<std::int64_t>
subtract_integer(std::int64_t a, std::int64_t b) noexcept {
    constexpr auto min = std::numeric_limits<std::int64_t>::min();
    constexpr auto max = std::numeric_limits<std::int64_t>::max();
    if ((b > 0 && a < min + b) || (b < 0 && a > max + b)) {
        return std::nullopt;
    }
    return a - b;
}

[[nodiscard]] constexpr std::optional<std::int64_t>
multiply_integer(std::int64_t a, std::int64_t b) noexcept {
    constexpr auto min = std::numeric_limits<std::int64_t>::min();
    constexpr auto max = std::numeric_limits<std::int64_t>::max();
    if (a == 0 || b == 0) {
        return 0;
    }
    if ((a > 0 && b > 0 && a > max / b) || (a > 0 && b < 0 && b < min / a) ||
        (a < 0 && b > 0 && a < min / b) || (a < 0 && b < 0 && a < max / b)) {
        return std::nullopt;
    }
    return a * b;
}
} // namespace agora::detail

// Currency-qualified cash arithmetic.
export namespace agora {
enum class MoneyError {
    unknown_currency,
    unknown_denomination,
    currency_mismatch,
    overflow,
    invalid_price,
    invalid_quantity,
    invalid_tick_value
};

class Money {
public:
    constexpr Money() noexcept = default; // Zero Attic drachmas.

    [[nodiscard]] static constexpr std::expected<Money, MoneyError>
    from_atoms(Currency currency, CashAtoms atoms) noexcept {
        if (!currency_definition(currency)) {
            return std::unexpected(MoneyError::unknown_currency);
        }
        return Money{currency, atoms};
    }

    [[nodiscard]] static constexpr std::expected<Money, MoneyError>
    from_denomination(Currency currency, Denomination denomination, std::int64_t count) noexcept {
        if (!currency_definition(currency)) {
            return std::unexpected(MoneyError::unknown_currency);
        }
        const auto scale = denomination_atoms(currency, denomination);
        if (!scale) {
            return std::unexpected(MoneyError::unknown_denomination);
        }
        const auto atoms = detail::multiply_integer(count, *scale);
        if (!atoms) {
            return std::unexpected(MoneyError::overflow);
        }
        return Money{currency, CashAtoms{*atoms}};
    }

    [[nodiscard]] constexpr Currency currency() const noexcept { return currency_; }
    [[nodiscard]] constexpr CashAtoms atoms() const noexcept { return atoms_; }
    constexpr bool operator==(const Money&) const noexcept = default;

private:
    constexpr Money(Currency currency, CashAtoms atoms) noexcept : currency_(currency), atoms_(atoms) {}
    Currency currency_{Currency::attic_drachma};
    CashAtoms atoms_{};
};

[[nodiscard]] constexpr std::expected<Money, MoneyError>
checked_add(Money a, Money b) noexcept {
    if (a.currency() != b.currency()) {
        return std::unexpected(MoneyError::currency_mismatch);
    }
    const auto sum = detail::add_integer(a.atoms().value(), b.atoms().value());
    if (!sum) {
        return std::unexpected(MoneyError::overflow);
    }
    return Money::from_atoms(a.currency(), CashAtoms{*sum});
}

[[nodiscard]] constexpr std::expected<Money, MoneyError>
checked_subtract(Money a, Money b) noexcept {
    if (a.currency() != b.currency()) {
        return std::unexpected(MoneyError::currency_mismatch);
    }
    const auto difference = detail::subtract_integer(a.atoms().value(), b.atoms().value());
    if (!difference) {
        return std::unexpected(MoneyError::overflow);
    }
    return Money::from_atoms(a.currency(), CashAtoms{*difference});
}

[[nodiscard]] constexpr std::expected<Money, MoneyError>
checked_scale(Money value, std::int64_t factor) noexcept {
    const auto product = detail::multiply_integer(value.atoms().value(), factor);
    if (!product) {
        return std::unexpected(MoneyError::overflow);
    }
    return Money::from_atoms(value.currency(), CashAtoms{*product});
}

// tick_value is the quote-currency value of ONE price tick for ONE whole lot.
// No FX conversion or rounding is performed; every amount is an exact integer.
[[nodiscard]] constexpr std::expected<Money, MoneyError>
checked_notional(Price price, Quantity lots, Money tick_value) noexcept {
    if (price.value() <= 0) {
        return std::unexpected(MoneyError::invalid_price);
    }
    if (lots.value() == 0) {
        return std::unexpected(MoneyError::invalid_quantity);
    }
    if (tick_value.atoms().value() <= 0) {
        return std::unexpected(MoneyError::invalid_tick_value);
    }
    const auto per_lot = detail::multiply_integer(price.value(), tick_value.atoms().value());
    if (!per_lot || lots.value() > static_cast<std::uint64_t>(
            std::numeric_limits<std::int64_t>::max() / *per_lot)) {
        return std::unexpected(MoneyError::overflow);
    }
    return Money::from_atoms(tick_value.currency(),
                            CashAtoms{*per_lot * static_cast<std::int64_t>(lots.value())});
}
} // namespace agora

// Venue metadata.
export namespace agora {
// Static metadata only: no exchange sessions, time-zone conversion, or routing yet.
struct VenueDefinition {
    VenueId id;
    std::string_view code;
    std::string_view name;
    std::string_view time_zone;
    Currency default_quote_currency;
};

inline constexpr VenueDefinition athens{
    VenueId{1}, "ATH", "Agora Athens", "Europe/Athens", Currency::attic_drachma};
} // namespace agora

// Commands and market events.
export namespace agora {
// Proposed first interface: limit orders, good till cancelled, one symbol per book.
struct NewOrder {
    OrderId id{};
    ParticipantId participant{};
    SymbolId symbol{};
    Side side{Side::buy};
    Price price{};
    Quantity quantity{};
};

struct CancelOrder {
    OrderId id{};
    ParticipantId participant{};
    SymbolId symbol{};
};

using Command = std::variant<NewOrder, CancelOrder>;

struct OrderAccepted { OrderId id{}; };
struct OrderRejected { OrderId id{}; std::string reason; };
struct OrderCancelled { OrderId id{}; Quantity remaining{}; };
struct Trade {
    OrderId maker{};
    OrderId taker{};
    Price price{};
    Quantity quantity{};
};

using MarketEvent = std::variant<OrderAccepted, OrderRejected, OrderCancelled, Trade>;
} // namespace agora
