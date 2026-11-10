module;
#include <optional>

export module agora.simulation;
export import agora.core;

export namespace agora::simulation {
struct ScheduledCommand {
    SimTime time{};
    Sequence sequence{};
    Command command;
};

class EventScheduler {
public:
    void schedule(ScheduledCommand event);
    [[nodiscard]] std::optional<ScheduledCommand> next();
    // TODO: order by (time, sequence), reject time regression, define sequence ownership.
};
} // namespace agora::simulation
