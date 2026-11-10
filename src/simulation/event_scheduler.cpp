module;
#include <stdexcept>
#include <optional>

module agora.simulation;

namespace agora::simulation {
void EventScheduler::schedule(ScheduledCommand) {
    throw std::logic_error("TODO: implement EventScheduler::schedule");
}

std::optional<ScheduledCommand> EventScheduler::next() {
    throw std::logic_error("TODO: implement EventScheduler::next");
}
} // namespace agora::simulation
