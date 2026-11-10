#include <array>
#include <chrono>
#include <concepts>
#include <cstdint>
#include <expected>
#include <iostream>
#include <limits>
#include <source_location>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>

import agora.core;

namespace {
using namespace agora;
using namespace std::chrono_literals;
constexpr auto min = std::numeric_limits<std::int64_t>::min();
constexpr auto max = std::numeric_limits<std::int64_t>::max();
constexpr auto attic = Currency::attic_drachma;
constexpr auto aeginetic = Currency::aeginetic_drachma;

template <class A, class B>
concept Addable = requires(A a, B b) { a + b; };

static_assert(!std::same_as<OrderId, ParticipantId>);
static_assert(!std::same_as<Price, Quantity>);
static_assert(!std::convertible_to<OrderId, std::uint64_t>);
static_assert(!std::convertible_to<ParticipantId, OrderId>);
static_assert(!std::convertible_to<UtcTimestamp, SimTime>);
static_assert(!std::convertible_to<SimTime, UtcTimestamp>);
static_assert(!std::constructible_from<SimTime, Nanoseconds>);
static_assert(!Addable<Price, Quantity>);
static_assert(sizeof(OrderId) == sizeof(std::uint64_t));
static_assert(sizeof(Price) == sizeof(std::int64_t));
static_assert(std::is_trivially_copyable_v<OrderId>);
static_assert(SimTime{}.elapsed() == 0ns);
static_assert(make_date(2000, 2, 29).has_value());
static_assert(!make_date(1900, 2, 29).has_value());
static_assert(*Money::from_denomination(attic, Denomination::drachma, 1) ==
              *Money::from_denomination(attic, Denomination::obol, 6));

void check(bool condition, const char* expression, std::source_location location) {
    if (!condition) {
        throw std::runtime_error(std::string{location.file_name()} + ':' +
                                 std::to_string(location.line()) + ": " + expression);
    }
}
#define CHECK(...) check(static_cast<bool>((__VA_ARGS__)), #__VA_ARGS__, std::source_location::current())

template <class T, class E>
void expect_error(const std::expected<T, E>& result, E expected) {
    CHECK(!result);
    CHECK(result.error() == expected);
}

Money atoms(std::int64_t value, Currency currency = attic) {
    const auto result = Money::from_atoms(currency, CashAtoms{value});
    CHECK(result);
    return *result;
}

void identifiers_and_messages() {
    const NewOrder order{OrderId{7}, ParticipantId{11}, SymbolId{3}, Side::buy,
                         Price{125}, Quantity{4}};
    CHECK(order.id.value() == 7);
    CHECK(order.price.value() == 125);
    CHECK(OrderId{}.value() == 0);
    CHECK(OrderId{1} < OrderId{2});
    std::unordered_map<OrderId, Quantity> quantities;
    quantities.emplace(order.id, order.quantity);
    CHECK(quantities.at(OrderId{7}) == Quantity{4});
    CHECK(athens.id == VenueId{1});
    CHECK(athens.time_zone == "Europe/Athens");
    CHECK(athens.default_quote_currency == attic);
    CHECK((MarketId{VenueId{1}, SymbolId{3}} != MarketId{VenueId{2}, SymbolId{3}}));
}

void calendar_and_time_of_day() {
    CHECK(make_date(2024, 2, 29));
    CHECK(make_date(0, 1, 1)); // Proleptic Gregorian astronomical year zero.
    CHECK(make_date(-500, 1, 1));
    expect_error(make_date(2025, 2, 29), TimeError::invalid_date);
    expect_error(make_date(2026, 4, 31), TimeError::invalid_date);
    expect_error(make_date(2026, 258, 1), TimeError::invalid_date);
    expect_error(make_date(2026, 1, 257), TimeError::invalid_date);
    expect_error(make_date(65'536, 1, 1), TimeError::invalid_date);
    CHECK(TimeOfDay::from_hms(23, 59, 59, 999'999'999)->since_midnight() == 24h - 1ns);
    expect_error(TimeOfDay::from_hms(24, 0, 0), TimeError::invalid_time_of_day);
    expect_error(TimeOfDay::from_hms(0, 60, 0), TimeError::invalid_time_of_day);
    expect_error(TimeOfDay::from_hms(0, 0, 60), TimeError::invalid_time_of_day);
    expect_error(TimeOfDay::from_hms(0, 0, 0, 1'000'000'000), TimeError::invalid_time_of_day);
    expect_error(TimeOfDay::from_elapsed(-1ns), TimeError::invalid_time_of_day);
    expect_error(TimeOfDay::from_elapsed(24h), TimeError::invalid_time_of_day);
}

void utc_boundaries() {
    const auto epoch = make_utc_timestamp(*make_date(1970, 1, 1));
    CHECK(epoch);
    CHECK(unix_ns(*epoch) == 0);
    const auto before_epoch = split_utc_timestamp(utc_from_unix_ns(-1));
    CHECK(before_epoch.date == *make_date(1969, 12, 31));
    CHECK(before_epoch.time.since_midnight() == 24h - 1ns);

    constexpr std::array<std::int64_t, 11> values{
        min, min + 1, -86'400'000'000'001, -86'400'000'000'000, -1, 0, 1,
        86'399'999'999'999, 86'400'000'000'000, max - 1, max};
    for (const auto value : values) {
        const auto calendar = split_utc_timestamp(utc_from_unix_ns(value));
        const auto round_trip = make_utc_timestamp(calendar.date, calendar.time);
        CHECK(round_trip);
        CHECK(unix_ns(*round_trip) == value);
    }
    const auto first = split_utc_timestamp(utc_from_unix_ns(min));
    const auto last = split_utc_timestamp(utc_from_unix_ns(max));
    const auto too_early = TimeOfDay::from_elapsed(first.time.since_midnight() - 1ns);
    const auto too_late = TimeOfDay::from_elapsed(last.time.since_midnight() + 1ns);
    CHECK(too_early);
    CHECK(too_late);
    expect_error(make_utc_timestamp(first.date, *too_early), TimeError::timestamp_out_of_range);
    expect_error(make_utc_timestamp(last.date, *too_late), TimeError::timestamp_out_of_range);
    expect_error(make_utc_timestamp(*make_date(1600, 1, 1)), TimeError::timestamp_out_of_range);
    expect_error(make_utc_timestamp(*make_date(9999, 12, 31)), TimeError::timestamp_out_of_range);
    expect_error(make_utc_timestamp(Date{}), TimeError::invalid_date);
}

void simulation_time() {
    const auto begin = *SimTime::from_elapsed(10ns);
    const auto end = advance_sim_time(begin, 5ns);
    CHECK(end);
    CHECK(end->elapsed() == 15ns);
    CHECK(*elapsed_between(begin, *end) == 5ns);
    CHECK(*advance_sim_time(begin, 0ns) == begin);
    expect_error(SimTime::from_elapsed(-1ns), TimeError::negative_duration);
    expect_error(advance_sim_time(begin, -1ns), TimeError::negative_duration);
    expect_error(elapsed_between(*end, begin), TimeError::time_regression);
    const auto last = *SimTime::from_elapsed(Nanoseconds{max});
    CHECK(*advance_sim_time(last, 0ns) == last);
    expect_error(advance_sim_time(last, 1ns), TimeError::overflow);
}

void currencies_and_denominations() {
    CHECK(currency_definition(attic)->code == "ATT");
    CHECK(currency_definition(aeginetic)->code == "AEG");
    CHECK(!currency_definition(static_cast<Currency>(0)));
    CHECK(*Money::from_denomination(attic, Denomination::drachma, 1) ==
          *Money::from_denomination(attic, Denomination::obol, 6));
    CHECK(*Money::from_denomination(attic, Denomination::obol, 1) ==
          *Money::from_denomination(attic, Denomination::hemiobol, 2));
    CHECK(*Money::from_denomination(attic, Denomination::tetradrachm, 1) ==
          *Money::from_denomination(attic, Denomination::drachma, 4));
    CHECK(Money::from_denomination(attic, Denomination::drachma, -1)->atoms() == CashAtoms{-6'000'000});
    expect_error(Money::from_atoms(static_cast<Currency>(0), CashAtoms{1}), MoneyError::unknown_currency);
    expect_error(Money::from_denomination(attic, static_cast<Denomination>(255), 1),
                 MoneyError::unknown_denomination);
    expect_error(Money::from_denomination(attic, Denomination::drachma, max), MoneyError::overflow);
    expect_error(checked_add(atoms(0), atoms(0, aeginetic)), MoneyError::currency_mismatch);
    expect_error(checked_subtract(atoms(5), atoms(3, aeginetic)), MoneyError::currency_mismatch);
}

void cash_arithmetic() {
    CHECK(checked_add(atoms(3), atoms(4))->atoms() == CashAtoms{7});
    CHECK(checked_subtract(atoms(3), atoms(4))->atoms() == CashAtoms{-1});
    CHECK(checked_subtract(atoms(min), atoms(min))->atoms() == CashAtoms{0});
    CHECK(checked_scale(atoms(min), 1)->atoms() == CashAtoms{min});
    CHECK(checked_scale(atoms(max), 1)->atoms() == CashAtoms{max});
    CHECK(checked_scale(atoms(min), 0)->atoms() == CashAtoms{0});
    CHECK(checked_scale(atoms(-2), -3)->atoms() == CashAtoms{6});
    CHECK(checked_scale(atoms(-2), 3)->atoms() == CashAtoms{-6});
    expect_error(checked_add(atoms(max), atoms(1)), MoneyError::overflow);
    expect_error(checked_add(atoms(min), atoms(-1)), MoneyError::overflow);
    expect_error(checked_subtract(atoms(min), atoms(1)), MoneyError::overflow);
    expect_error(checked_subtract(atoms(max), atoms(-1)), MoneyError::overflow);
    expect_error(checked_subtract(atoms(0), atoms(min)), MoneyError::overflow);
    expect_error(checked_scale(atoms(min), -1), MoneyError::overflow);
    expect_error(checked_scale(atoms(-1), min), MoneyError::overflow);
    expect_error(checked_scale(atoms(max), 2), MoneyError::overflow);
    expect_error(checked_scale(atoms(min), 2), MoneyError::overflow);
    constexpr std::array<std::int64_t, 5> small{-500, -1, 0, 1, 500};
    for (const auto a : small) {
        for (const auto b : small) {
            CHECK(checked_add(atoms(a), atoms(b))->atoms() == CashAtoms{a + b});
            CHECK(checked_subtract(atoms(a), atoms(b))->atoms() == CashAtoms{a - b});
            CHECK(checked_scale(atoms(a), b)->atoms() == CashAtoms{a * b});
        }
    }
}

void quote_notional() {
    const auto cost = checked_notional(Price{125}, Quantity{4}, atoms(1'000));
    CHECK(cost);
    CHECK(cost->atoms() == CashAtoms{500'000});
    CHECK(cost->currency() == attic);
    CHECK(checked_notional(Price{1}, Quantity{1}, atoms(max))->atoms() == CashAtoms{max});
    expect_error(checked_notional(Price{0}, Quantity{1}, atoms(1)), MoneyError::invalid_price);
    expect_error(checked_notional(Price{-1}, Quantity{1}, atoms(1)), MoneyError::invalid_price);
    expect_error(checked_notional(Price{1}, Quantity{0}, atoms(1)), MoneyError::invalid_quantity);
    expect_error(checked_notional(Price{1}, Quantity{1}, atoms(0)), MoneyError::invalid_tick_value);
    expect_error(checked_notional(Price{1}, Quantity{1}, atoms(-1)), MoneyError::invalid_tick_value);
    expect_error(checked_notional(Price{max}, Quantity{2}, atoms(1)), MoneyError::overflow);
    expect_error(checked_notional(Price{max}, Quantity{1}, atoms(2)), MoneyError::overflow);
    expect_error(checked_notional(Price{1}, Quantity{std::numeric_limits<std::uint64_t>::max()}, atoms(1)),
                 MoneyError::overflow);
}
} // namespace

int main() {
    try {
        identifiers_and_messages();
        calendar_and_time_of_day();
        utc_boundaries();
        simulation_time();
        currencies_and_denominations();
        cash_arithmetic();
        quote_notional();
        std::cout << "Core foundation checks passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
