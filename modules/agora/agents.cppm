module;
#include <vector>

export module agora.agents;
export import agora.core;

export namespace agora::agents
{
// The controller supplies only events visible to this participant.
// Each implementation owns its strategy state and seeded random generator.
class Agent
    {
public:
    virtual ~Agent() = default;
    [[nodiscard]] virtual ParticipantId id() const noexcept = 0;
    [[nodiscard]] virtual std::vector<Command> on_event(SimTime now,
                                                       const MarketEvent& event) = 0;
    };
} // namespace agora::agents
